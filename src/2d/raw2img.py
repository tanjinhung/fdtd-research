import argparse
import re
import glob
import sys
import os
from PIL import Image
import numpy as np
import matplotlib.pyplot as plt


def raw2image(filename, z_norm=1.0, decades=3, cmap=None):
    """
    Python translation of the MATLAB raw2image function.
    """
    if cmap is None:
        cmap = plt.get_cmap("jet", 128)
    # read file
    with open(filename, "rb") as f:
        header = f.read(8)
        if len(header) < 8:
            raise ValueError(f"[raw2image] Could not read header from {filename}")
        size_x, size_y = np.frombuffer(header, dtype=np.float32)
        size_x = int(size_x)
        size_y = int(size_y)
        data = np.frombuffer(f.read(), dtype=np.float32)
    expected = size_x * size_y
    if data.size < expected:
        raise ValueError("[raw2image] File does not contain enough data")
    data = data[:expected]
    data = data.reshape((size_x, size_y), order="F").T[::-1, :]

    tiny = np.finfo(np.float32).tiny
    if decades != 0:
        img = np.log10(np.abs((data + tiny) / z_norm))
        vmin, vmax = -decades, 0
    else:
        img = np.abs((data + tiny) / z_norm)
        vmin, vmax = 0, 1

    plt.figure()
    plt.imshow(
        img,
        cmap=cmap,
        origin="lower",
        extent=(1, size_x, 1, size_y),
        vmin=vmin,
        vmax=vmax,
        aspect="equal",
    )
    plt.colorbar()
    plt.show()


def _read_raw(path):
    """
    Read the raw file and return the data.
    """
    with open(path, "rb") as f:
        header = f.read(8)
        if len(header) < 8:
            raise ValueError(f"[raw2gif] Could not read Header from {path}")
        hdr = np.frombuffer(header, dtype=np.float32)
        if hdr.size < 2:
            raise ValueError(f"[raw2gif] Header is less than 2 in {path}")
        x = int(hdr[0])
        y = int(hdr[1])
        data = np.frombuffer(f.read(), dtype=np.float32)

    expected = x * y
    if data.size < expected:
        raise ValueError(f"[raw2gif] File {path} contains incorrect data")

    data = data[:expected]
    data = data.reshape((x, y), order="F").T[::-1, :]
    return data, x, y


def natural_key(path):
    name = os.path.basename(path)
    parts = re.split(r"(\d+)", name)
    return tuple((0, int(p)) if p.isdigit() else (1, p.lower()) for p in parts)


def raw2gif(
    absolute_path,
    out_file,
    z_norm=1.0,
    decades=3,
    cmap="jet",
    duration=0.1,
    pattern="sim.*",
    loop=0,
    save_frames_to_dir=None,
    optimize=True,
    verbose=False,
):
    """
    Converts a series of data files into a gif
    - absolute_path : The absolute path to the folder
    - out_file      : The filename of the output gif file
    - z_norm        : Value used to normalize all frames, typically the maximum value of all the frame (Default = 1.0)
    - decades       : Decades to be used to display. Normalized data is assumed to vary between 1.0 and 10^(-decades) such that after taking the log_10, the values vary between 0 ~ -decades. (Default = 3)
    - cmap          : Matplotlib colormap (Default = plt.get_cmap("jet", 128))
    - duration      : Seconds per frame (Default = 0.1)
    - pattern       : Glob pattern to match the raw file (Default = "sim.*")
    - loop          : The loop count of the gif (0 = infinite) (Default = 0)
    """
    if isinstance(cmap, str):
        cmap = plt.get_cmap(cmap)

    paths = sorted(glob.glob(os.path.join(absolute_path, pattern)), key=natural_key)
    if verbose:
        print(f"[raw2gif] matched {len(paths)} files")

    if not paths:
        raise FileNotFoundError(
            f"[raw2gif] No files matching {pattern} in {absolute_path}"
        )

    if save_frames_to_dir:
        os.makedirs(save_frames_to_dir, exist_ok=True)

    tiny = np.finfo(np.float32).tiny
    frames = []

    for idx, p in enumerate(paths):
        if verbose:
            print(f"[raw2gif] reading {p}")

        data, sx, sy = _read_raw(p)
        if decades != 0:
            img = np.log10(np.abs(data + tiny) / z_norm)
            vmin, vmax = -decades, 0
        else:
            img = np.abs(data + tiny) / z_norm
            vmin, vmax = 0, 1

        denom = vmax - vmin
        if denom == 0:
            denom = 1.0

        img_norm = (img - vmin) / denom
        img_norm = np.clip(img_norm, 0.0, 1.0)

        rgba = cmap(img_norm)
        rgb = (rgba[:, :, :3] * 255).astype(np.uint8)
        im = Image.fromarray(rgb)

        if save_frames_to_dir:
            base = os.path.splitext(os.path.basename(p))[0] + ".png"
            im.save(os.path.join(save_frames_to_dir, base))

        palette_img = im.quantize(colors=256)
        frames.append(palette_img)

    first, *others = frames
    out_dir = os.path.dirname(os.path.abspath(out_file))
    os.makedirs(out_dir, exist_ok=True)

    if verbose:
        print(f"[raw2gif] saving {out_file} ({len(frames)} frames)")

    first.save(
        out_file,
        format="GIF",
        save_all=True,
        append_images=others,
        duration=int(duration * 1000),
        loop=loop,
        optimize=optimize,
    )

    if verbose:
        print("[raw2gif] done.")


def _parse_args(argv):
    p = argparse.ArgumentParser(
        description="Convert a folder of raw frames to an animated gif"
    )

    p.add_argument(
        "-i",
        "--input",
        required=True,
        help="input folder containing raw files",
    )
    p.add_argument(
        "out_file",
        help="output GIF path",
    )
    p.add_argument(
        "-p",
        "--pattern",
        default="sim.*",
        help='glob pattern to match files (default: "sim.*")',
    )
    p.add_argument(
        "--z_norm",
        type=float,
        default=1.0,
        help="normalization value (default: 1.0)",
    )
    p.add_argument(
        "--decades",
        type=float,
        default=3.0,
        help="decades for log scaling (0 => linear)",
    )
    p.add_argument(
        "--cmap",
        default="jet",
        help="matplotlib colormap name (default: jet)",
    )
    p.add_argument(
        "-t",
        "--duration",
        type=float,
        default=0.1,
        help="seconds per frame (default: 0.1)",
    )
    p.add_argument(
        "-l",
        "--loop",
        type=int,
        default=0,
        help="GIF Loop Count (0 => infinite)",
    )
    p.add_argument(
        "--save-frames-to-dir",
        default=None,
        help="directory to save intermediate PNG frame (optional)",
    )
    p.add_argument(
        "--no-optimize", action="store_true", help="disable Pillow GIF optimization"
    )
    p.add_argument("-v", "--verbose", action="store_true", help="verbose output")
    return p.parse_args(argv)


def main(argv=None):
    args = _parse_args(argv or sys.argv[1:])
    try:
        raw2gif(
            absolute_path=args.input,
            out_file=args.out_file,
            z_norm=args.z_norm,
            decades=args.decades,
            cmap=args.cmap,
            duration=args.duration,
            pattern=args.pattern,
            loop=args.loop,
            save_frames_to_dir=args.save_frames_to_dir,
            optimize=not args.no_optimize,
            verbose=args.verbose,
        )
    except Exception as e:
        print(f"[raw2gif] error: {e}", file=sys.stderr)
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
