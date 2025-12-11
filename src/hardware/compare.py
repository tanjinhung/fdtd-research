import matplotlib.pyplot as plt
import numpy as np

import re # Import the regular expression module

def parse_data_string(data_string):
    """Parses a multi-line string of data into a 2D numpy array."""
    lines = data_string.strip().split('\n')
    data = {}
    
    # Regular expression to match the pattern: [ R, C, 42] float: VALUE
    # It captures R, C, and VALUE
    # It handles potential extra spaces.
    pattern = re.compile(r'\[\s*(\d+),\s*(\d+),\s*42\]\s*float:\s*([-\d.]+)')

    max_row_idx_found = 0
    max_col_idx_found = 0

    for line in lines:
        match = pattern.search(line)
        if match:
            try:
                row_1_based = int(match.group(1))
                col_1_based = int(match.group(2))
                float_val = float(match.group(3))
                
                # Adjust to 0-based indices
                row_0_based = row_1_based - 1
                col_0_based = col_1_based - 1
                
                data[(row_0_based, col_0_based)] = float_val
                
                # Keep track of the maximum 0-based index found
                if row_0_based > max_row_idx_found:
                    max_row_idx_found = row_0_based
                if col_0_based > max_col_idx_found:
                    max_col_idx_found = col_0_based

            except (ValueError, IndexError) as e:
                print(f"Skipping malformed line (regex matched, but conversion failed): {line} (Error: {e})")
        else:
            # print(f"Skipping line (does not match expected format): {line}") # Uncomment to see skipped lines
            pass

    # Determine matrix dimensions based on the max indices found
    # Add 1 because indices are 0-based (e.g., max_idx 29 means 30 rows/cols)
    rows_dim = max_row_idx_found + 1 if data else 0
    cols_dim = max_col_idx_found + 1 if data else 0
    
    if rows_dim == 0 or cols_dim == 0:
        print("Error: No valid data parsed. Returning empty matrix.")
        return np.array([]) # Return empty numpy array to indicate failure

    if not (rows_dim == 30 and cols_dim == 30):
        print(f"Warning: Expected 30x30 matrix, but parsed data implies dimensions {rows_dim}x{cols_dim}. This might be fine if the actual data size is different.")

    matrix = np.zeros((rows_dim, cols_dim))
    for (r, c), val in data.items():
        # This check is now less critical since dimensions are derived from max_idx_found
        # but good for safety if max_idx_found was somehow off or sparse data
        if r < rows_dim and c < cols_dim: 
            matrix[r, c] = val
    return matrix

def visualize_3d_fields(data_string1, data_string2):
    """
    Visualizes two datasets as 3D surface plots where float is the height.

    Args:
        data_string1 (str): Multi-line string of the first dataset.
        data_string2 (str): Multi-line string of the second dataset.
    """
    matrix1 = parse_data_string(data_string1)
    matrix2 = parse_data_string(data_string2)

    if matrix1.size == 0 or matrix2.size == 0:
        print("Error: Could not parse enough data to create matrices for visualization.")
        return

    rows, cols = matrix1.shape
    X = np.arange(0, cols)
    Y = np.arange(0, rows)
    X, Y = np.meshgrid(X, Y)

    fig = plt.figure(figsize=(18, 8))

    # Plot for Data String 1
    ax1 = fig.add_subplot(121, projection='3d')
    ax1.plot_surface(X, Y, matrix1, cmap='plasma')
    ax1.set_title('Dataset 1: Float Value as Height')
    ax1.set_xlabel('Column')
    ax1.set_ylabel('Row')
    ax1.set_zlabel('Float Value (Height)')

    # Plot for Data String 2
    ax2 = fig.add_subplot(122, projection='3d')
    ax2.plot_surface(X, Y, matrix2, cmap='plasma')
    ax2.set_title('Dataset 2: Float Value as Height')
    ax2.set_xlabel('Column')
    ax2.set_ylabel('Row')
    ax2.set_zlabel('Float Value (Height)')

    plt.tight_layout()
    plt.show()

# --- How to use the script with your data ---
# Replace 'your_data_string_1' and 'your_data_string_2' with your actual data.
# The data strings should be exactly as you provided them in your previous turns.

# Example usage (you will replace these with your actual full data strings)
data_str_1 = '''
[ 1,  1, 42] float:  0.001637 byte 12 87 D6 3A 
[ 1,  2, 42] float: -0.008955 byte 14 BA 12 BC 
[ 1,  3, 42] float: -0.009267 byte 21 D3 17 BC 
[ 1,  4, 42] float: -0.003373 byte 7E 06 5D BB 
[ 1,  5, 42] float: -0.000398 byte A8 B4 D0 B9 
[ 1,  6, 42] float:  0.005844 byte 9E 7B BF 3B 
[ 1,  7, 42] float:  0.007539 byte B4 0D F7 3B 
[ 1,  8, 42] float:  0.008775 byte 19 C3 0F 3C 
[ 1,  9, 42] float:  0.007525 byte 9B 93 F6 3B 
[ 1, 10, 42] float:  0.005819 byte F6 B0 BE 3B 
[ 1, 11, 42] float:  0.003576 byte 68 5B 6A 3B 
[ 1, 12, 42] float:  0.001241 byte 64 A4 A2 3A 
[ 1, 13, 42] float: -0.000088 byte 00 81 B7 B8 
[ 1, 14, 42] float: -0.001965 byte EC C4 00 BB 
[ 1, 15, 42] float: -0.001969 byte 34 02 01 BB 
[ 1, 16, 42] float: -0.003033 byte BE C6 46 BB 
[ 1, 17, 42] float: -0.001969 byte 34 02 01 BB 
[ 1, 18, 42] float: -0.001965 byte EC C4 00 BB 
[ 1, 19, 42] float: -0.000088 byte 00 81 B7 B8 
[ 1, 20, 42] float:  0.001241 byte 64 A4 A2 3A 
[ 1, 21, 42] float:  0.003576 byte 68 5B 6A 3B 
[ 1, 22, 42] float:  0.005819 byte F6 B0 BE 3B 
[ 1, 23, 42] float:  0.007525 byte 9B 93 F6 3B 
[ 1, 24, 42] float:  0.008775 byte 19 C3 0F 3C 
[ 1, 25, 42] float:  0.007539 byte B4 0D F7 3B 
[ 1, 26, 42] float:  0.005844 byte 9E 7B BF 3B 
[ 1, 27, 42] float: -0.000398 byte A8 B4 D0 B9 
[ 1, 28, 42] float: -0.004084 byte 7B D4 85 BB 
[ 1, 29, 42] float: -0.010311 byte 6C F0 28 BC 
[ 1, 30, 42] float: -0.011943 byte 85 AE 43 BC 
[ 2,  1, 42] float: -0.008955 byte 14 BA 12 BC 
[ 2,  2, 42] float: -0.008491 byte 9C 1C 0B BC 
[ 2,  3, 42] float: -0.006652 byte 68 FC D9 BB 
[ 2,  4, 42] float: -0.001562 byte 7A BE CC BA 
[ 2,  5, 42] float:  0.000330 byte 18 C8 AC 39 
[ 2,  6, 42] float:  0.003035 byte AD EE 46 3B 
[ 2,  7, 42] float:  0.007154 byte 5A 68 EA 3B 
[ 2,  8, 42] float:  0.004492 byte 0C 34 93 3B 
[ 2,  9, 42] float:  0.007195 byte 0C C8 EB 3B 
[ 2, 10, 42] float:  0.002848 byte A8 A6 3A 3B 
[ 2, 11, 42] float:  0.002963 byte BA 28 42 3B 
[ 2, 12, 42] float:  0.000535 byte 28 40 0C 3A 
[ 2, 13, 42] float: -0.000780 byte 58 94 4C BA 
[ 2, 14, 42] float: -0.000957 byte C4 CA 7A BA 
[ 2, 15, 42] float: -0.002575 byte 5A BB 28 BB 
[ 2, 16, 42] float: -0.001430 byte 24 6B BB BA 
[ 2, 17, 42] float: -0.002575 byte 5A BB 28 BB 
[ 2, 18, 42] float: -0.000957 byte C4 CA 7A BA 
[ 2, 19, 42] float: -0.000780 byte 58 94 4C BA 
[ 2, 20, 42] float:  0.000535 byte 28 40 0C 3A 
[ 2, 21, 42] float:  0.002963 byte BA 28 42 3B 
[ 2, 22, 42] float:  0.002848 byte A8 A6 3A 3B 
[ 2, 23, 42] float:  0.007195 byte 0C C8 EB 3B 
[ 2, 24, 42] float:  0.004492 byte 0C 34 93 3B 
[ 2, 25, 42] float:  0.007154 byte 5A 68 EA 3B 
[ 2, 26, 42] float:  0.003035 byte AD EE 46 3B 
[ 2, 27, 42] float: -0.000232 byte 70 81 73 B9 
[ 2, 28, 42] float: -0.002433 byte 90 6B 1F BB 
[ 2, 29, 42] float: -0.010233 byte 65 AA 27 BC 
[ 2, 30, 42] float: -0.011243 byte A4 36 38 BC 
[ 3,  1, 42] float: -0.009267 byte 21 D3 17 BC 
[ 3,  2, 42] float: -0.006652 byte 68 FC D9 BB 
[ 3,  3, 42] float:  0.001351 byte 06 18 B1 3A 
[ 3,  4, 42] float: -0.002122 byte BC 0B 0B BB 
[ 3,  5, 42] float: -0.000813 byte EE 09 55 BA 
[ 3,  6, 42] float:  0.002800 byte 14 7D 37 3B 
[ 3,  7, 42] float: -0.000721 byte F8 E3 3C BA 
[ 3,  8, 42] float:  0.001265 byte AC CC A5 3A 
[ 3,  9, 42] float: -0.001269 byte 7C 64 A6 BA 
[ 3, 10, 42] float: -0.002357 byte DC 75 1A BB 
[ 3, 11, 42] float: -0.001482 byte CC 40 C2 BA 
[ 3, 12, 42] float: -0.003275 byte C3 A3 56 BB 
[ 3, 13, 42] float: -0.001018 byte 59 6D 85 BA 
[ 3, 14, 42] float: -0.002641 byte 53 10 2D BB 
[ 3, 15, 42] float: -0.000581 byte 50 5A 18 BA 
[ 3, 16, 42] float: -0.002244 byte 0D 16 13 BB 
[ 3, 17, 42] float: -0.000581 byte 50 5A 18 BA 
[ 3, 18, 42] float: -0.002641 byte 53 10 2D BB 
[ 3, 19, 42] float: -0.001018 byte 59 6D 85 BA 
[ 3, 20, 42] float: -0.003275 byte C3 A3 56 BB 
[ 3, 21, 42] float: -0.001482 byte CC 40 C2 BA 
[ 3, 22, 42] float: -0.002357 byte DC 75 1A BB 
[ 3, 23, 42] float: -0.001269 byte 7C 64 A6 BA 
[ 3, 24, 42] float:  0.001265 byte AC CC A5 3A 
[ 3, 25, 42] float: -0.000721 byte F8 E3 3C BA 
[ 3, 26, 42] float:  0.002407 byte 82 B7 1D 3B 
[ 3, 27, 42] float: -0.001454 byte 2A 97 BE BA 
[ 3, 28, 42] float: -0.005225 byte 09 33 AB BB 
[ 3, 29, 42] float: -0.001318 byte 64 CA AC BA 
[ 3, 30, 42] float: -0.000441 byte E0 3E E7 B9 
[ 4,  1, 42] float: -0.003373 byte 7E 06 5D BB 
[ 4,  2, 42] float: -0.001562 byte 7A BE CC BA 
[ 4,  3, 42] float: -0.002122 byte BC 0B 0B BB 
[ 4,  4, 42] float: -0.000979 byte 3B 57 80 BA 
[ 4,  5, 42] float:  0.004557 byte 81 54 95 3B 
[ 4,  6, 42] float:  0.001636 byte 30 77 D6 3A 
[ 4,  7, 42] float: -0.004034 byte DE 2F 84 BB 
[ 4,  8, 42] float: -0.003430 byte 2A CD 60 BB 
[ 4,  9, 42] float: -0.009777 byte CC 2F 20 BC 
[ 4, 10, 42] float: -0.005716 byte DA 4C BB BB 
[ 4, 11, 42] float: -0.006672 byte 7C A4 DA BB 
[ 4, 12, 42] float: -0.004352 byte F0 98 8E BB 
[ 4, 13, 42] float: -0.001787 byte EC 34 EA BA 
[ 4, 14, 42] float: -0.002585 byte FE 61 29 BB 
[ 4, 15, 42] float:  0.000880 byte 20 B3 66 3A 
[ 4, 16, 42] float: -0.001910 byte 88 56 FA BA 
[ 4, 17, 42] float:  0.000880 byte 20 B3 66 3A 
[ 4, 18, 42] float: -0.002585 byte FE 61 29 BB 
[ 4, 19, 42] float: -0.001787 byte EC 34 EA BA 
[ 4, 20, 42] float: -0.004352 byte F0 98 8E BB 
[ 4, 21, 42] float: -0.006672 byte 7C A4 DA BB 
[ 4, 22, 42] float: -0.005716 byte DA 4C BB BB 
[ 4, 23, 42] float: -0.009777 byte CC 2F 20 BC 
[ 4, 24, 42] float: -0.003430 byte 2A CD 60 BB 
[ 4, 25, 42] float: -0.004277 byte F9 29 8C BB 
[ 4, 26, 42] float:  0.001219 byte C0 D1 9F 3A 
[ 4, 27, 42] float:  0.002117 byte 90 B5 0A 3B 
[ 4, 28, 42] float: -0.003360 byte 98 2E 5C BB 
[ 4, 29, 42] float:  0.001172 byte E6 AC 99 3A 
[ 4, 30, 42] float:  0.005142 byte 44 7E A8 3B 
[ 5,  1, 42] float: -0.000398 byte A8 B4 D0 B9 
[ 5,  2, 42] float:  0.000330 byte 18 C8 AC 39 
[ 5,  3, 42] float: -0.000813 byte EE 09 55 BA 
[ 5,  4, 42] float:  0.004557 byte 81 54 95 3B 
[ 5,  5, 42] float:  0.004354 byte B6 AC 8E 3B 
[ 5,  6, 42] float: -0.006221 byte BF DB CB BB 
[ 5,  7, 42] float: -0.006115 byte 1B 60 C8 BB 
[ 5,  8, 42] float: -0.010129 byte B8 F5 25 BC 
[ 5,  9, 42] float: -0.007493 byte EA 8B F5 BB 
[ 5, 10, 42] float: -0.003068 byte C8 08 49 BB 
[ 5, 11, 42] float: -0.003215 byte FA AC 52 BB 
[ 5, 12, 42] float:  0.002687 byte D4 15 30 3B 
[ 5, 13, 42] float: -0.000019 byte 80 D8 9E B7 
[ 5, 14, 42] float:  0.004597 byte 53 9F 96 3B 
[ 5, 15, 42] float:  0.001245 byte D5 35 A3 3A 
[ 5, 16, 42] float:  0.004918 byte A6 26 A1 3B 
[ 5, 17, 42] float:  0.001245 byte D5 35 A3 3A 
[ 5, 18, 42] float:  0.004597 byte 53 9F 96 3B 
[ 5, 19, 42] float: -0.000019 byte 80 D8 9E B7 
[ 5, 20, 42] float:  0.002687 byte D4 15 30 3B 
[ 5, 21, 42] float: -0.003215 byte FA AC 52 BB 
[ 5, 22, 42] float: -0.003068 byte C8 08 49 BB 
[ 5, 23, 42] float: -0.007493 byte EA 8B F5 BB 
[ 5, 24, 42] float: -0.010262 byte A7 22 28 BC 
[ 5, 25, 42] float: -0.006353 byte 9A 2E D0 BB 
[ 5, 26, 42] float: -0.007946 byte F8 2D 02 BC 
[ 5, 27, 42] float:  0.002448 byte 6D 72 20 3B 
[ 5, 28, 42] float:  0.005118 byte A6 B1 A7 3B 
[ 5, 29, 42] float:  0.002754 byte 6A 7F 34 3B 
[ 5, 30, 42] float:  0.006851 byte 5C 7D E0 3B 
[ 6,  1, 42] float:  0.005844 byte 9E 7B BF 3B 
[ 6,  2, 42] float:  0.003035 byte AD EE 46 3B 
[ 6,  3, 42] float:  0.002800 byte 14 7D 37 3B 
[ 6,  4, 42] float:  0.001636 byte 30 77 D6 3A 
[ 6,  5, 42] float: -0.006221 byte BF DB CB BB 
[ 6,  6, 42] float: -0.008171 byte BF E0 05 BC 
[ 6,  7, 42] float: -0.005866 byte 2D 35 C0 BB 
[ 6,  8, 42] float: -0.003309 byte 98 DB 58 BB 
[ 6,  9, 42] float:  0.001573 byte 98 29 CE 3A 
[ 6, 10, 42] float:  0.002800 byte 62 82 37 3B 
[ 6, 11, 42] float:  0.003541 byte CD 16 68 3B 
[ 6, 12, 42] float:  0.003740 byte CE 15 75 3B 
[ 6, 13, 42] float:  0.001531 byte DE A2 C8 3A 
[ 6, 14, 42] float:  0.002798 byte C3 64 37 3B 
[ 6, 15, 42] float:  0.000198 byte 00 9F 4F 39 
[ 6, 16, 42] float:  0.002358 byte 73 84 1A 3B 
[ 6, 17, 42] float:  0.000198 byte 00 9F 4F 39 
[ 6, 18, 42] float:  0.002798 byte C3 64 37 3B 
[ 6, 19, 42] float:  0.001531 byte DE A2 C8 3A 
[ 6, 20, 42] float:  0.003740 byte CE 15 75 3B 
[ 6, 21, 42] float:  0.003541 byte CD 16 68 3B 
[ 6, 22, 42] float:  0.002800 byte 62 82 37 3B 
[ 6, 23, 42] float:  0.001509 byte B8 D6 C5 3A 
[ 6, 24, 42] float: -0.003428 byte 16 AA 60 BB 
[ 6, 25, 42] float: -0.006948 byte CF AF E3 BB 
[ 6, 26, 42] float: -0.009517 byte F1 EB 1B BC 
[ 6, 27, 42] float: -0.007432 byte 44 84 F3 BB 
[ 6, 28, 42] float:  0.002485 byte 05 D7 22 3B 
[ 6, 29, 42] float:  0.009968 byte CE 51 23 3C 
[ 6, 30, 42] float:  0.007807 byte B4 D5 FF 3B 
[ 7,  1, 42] float:  0.007539 byte B4 0D F7 3B 
[ 7,  2, 42] float:  0.007154 byte 5A 68 EA 3B 
[ 7,  3, 42] float: -0.000721 byte F8 E3 3C BA 
[ 7,  4, 42] float: -0.004034 byte DE 2F 84 BB 
[ 7,  5, 42] float: -0.006115 byte 1B 60 C8 BB 
[ 7,  6, 42] float: -0.005866 byte 2D 35 C0 BB 
[ 7,  7, 42] float:  0.000252 byte 08 EA 83 39 
[ 7,  8, 42] float:  0.000912 byte 30 01 6F 3A 
[ 7,  9, 42] float:  0.003017 byte BC B2 45 3B 
[ 7, 10, 42] float:  0.002426 byte F0 04 1F 3B 
[ 7, 11, 42] float:  0.000610 byte 12 E0 1F 3A 
[ 7, 12, 42] float:  0.000033 byte 00 17 09 38 
[ 7, 13, 42] float: -0.001477 byte AF A1 C1 BA 
[ 7, 14, 42] float: -0.000532 byte 7E 8C 0B BA 
[ 7, 15, 42] float: -0.001895 byte C5 66 F8 BA 
[ 7, 16, 42] float: -0.000511 byte A2 DB 05 BA 
[ 7, 17, 42] float: -0.001895 byte C5 66 F8 BA 
[ 7, 18, 42] float: -0.000532 byte 7E 8C 0B BA 
[ 7, 19, 42] float: -0.001477 byte AF A1 C1 BA 
[ 7, 20, 42] float:  0.000033 byte 00 17 09 38 
[ 7, 21, 42] float:  0.000610 byte 12 E0 1F 3A 
[ 7, 22, 42] float:  0.002400 byte 02 49 1D 3B 
[ 7, 23, 42] float:  0.002965 byte D2 4D 42 3B 
[ 7, 24, 42] float:  0.000313 byte 7F 58 A4 39 
[ 7, 25, 42] float: -0.000575 byte 70 C8 16 BA 
[ 7, 26, 42] float: -0.007701 byte 42 57 FC BB 
[ 7, 27, 42] float: -0.006932 byte E6 27 E3 BB 
[ 7, 28, 42] float:  0.001100 byte DF 3B 90 3A 
[ 7, 29, 42] float:  0.005186 byte 78 EB A9 3B 
[ 7, 30, 42] float:  0.006048 byte BF 2B C6 3B 
[ 8,  1, 42] float:  0.008775 byte 19 C3 0F 3C 
[ 8,  2, 42] float:  0.004492 byte 0C 34 93 3B 
[ 8,  3, 42] float:  0.001265 byte AC CC A5 3A 
[ 8,  4, 42] float: -0.003430 byte 2A CD 60 BB 
[ 8,  5, 42] float: -0.010129 byte B8 F5 25 BC 
[ 8,  6, 42] float: -0.003309 byte 98 DB 58 BB 
[ 8,  7, 42] float:  0.000912 byte 30 01 6F 3A 
[ 8,  8, 42] float:  0.000385 byte C0 CD C9 39 
[ 8,  9, 42] float:  0.005521 byte 8E E6 B4 3B 
[ 8, 10, 42] float:  0.000424 byte 30 14 DE 39 
[ 8, 11, 42] float:  0.002039 byte A0 A0 05 3B 
[ 8, 12, 42] float:  0.000514 byte F4 BA 06 3A 
[ 8, 13, 42] float:  0.001252 byte 44 0E A4 3A 
[ 8, 14, 42] float:  0.001924 byte F6 2A FC 3A 
[ 8, 15, 42] float:  0.000948 byte 8E 6E 78 3A 
[ 8, 16, 42] float:  0.002457 byte C4 06 21 3B 
[ 8, 17, 42] float:  0.000948 byte 8E 6E 78 3A 
[ 8, 18, 42] float:  0.001924 byte F6 2A FC 3A 
[ 8, 19, 42] float:  0.001252 byte 44 0E A4 3A 
[ 8, 20, 42] float:  0.000514 byte F4 BA 06 3A 
[ 8, 21, 42] float:  0.002029 byte CD 00 05 3B 
[ 8, 22, 42] float:  0.000404 byte 70 E5 D3 39 
[ 8, 23, 42] float:  0.005233 byte BA 76 AB 3B 
[ 8, 24, 42] float: -0.000053 byte A0 B7 5D B8 
[ 8, 25, 42] float: -0.000722 byte 90 44 3D BA 
[ 8, 26, 42] float: -0.004683 byte 59 72 99 BB 
[ 8, 27, 42] float: -0.008127 byte 2C 28 05 BC 
[ 8, 28, 42] float:  0.000726 byte AA 56 3E 3A 
[ 8, 29, 42] float:  0.005405 byte A8 1F B1 3B 
[ 8, 30, 42] float:  0.002110 byte 2A 41 0A 3B 
[ 9,  1, 42] float:  0.007525 byte 9B 93 F6 3B 
[ 9,  2, 42] float:  0.007195 byte 0C C8 EB 3B 
[ 9,  3, 42] float: -0.001269 byte 7C 64 A6 BA 
[ 9,  4, 42] float: -0.009777 byte CC 2F 20 BC 
[ 9,  5, 42] float: -0.007493 byte EA 8B F5 BB 
[ 9,  6, 42] float:  0.001573 byte 98 29 CE 3A 
[ 9,  7, 42] float:  0.003017 byte BC B2 45 3B 
[ 9,  8, 42] float:  0.005521 byte 8E E6 B4 3B 
[ 9,  9, 42] float:  0.003321 byte A2 9D 59 3B 
[ 9, 10, 42] float: -0.001390 byte D2 39 B6 BA 
[ 9, 11, 42] float:  0.001292 byte FC 55 A9 3A 
[ 9, 12, 42] float: -0.002165 byte 23 E4 0D BB 
[ 9, 13, 42] float:  0.002086 byte BB BB 08 3B 
[ 9, 14, 42] float: -0.002155 byte 1E 40 0D BB 
[ 9, 15, 42] float:  0.002024 byte DA AC 04 3B 
[ 9, 16, 42] float: -0.002147 byte 3C AE 0C BB 
[ 9, 17, 42] float:  0.002024 byte DA AC 04 3B 
[ 9, 18, 42] float: -0.002155 byte 1E 40 0D BB 
[ 9, 19, 42] float:  0.002086 byte BB BB 08 3B 
[ 9, 20, 42] float: -0.002168 byte 51 15 0E BB 
[ 9, 21, 42] float:  0.001286 byte 7E 85 A8 3A 
[ 9, 22, 42] float: -0.001510 byte 3E E3 C5 BA 
[ 9, 23, 42] float:  0.003123 byte 7A A7 4C 3B 
[ 9, 24, 42] float:  0.004425 byte E1 FF 90 3B 
[ 9, 25, 42] float:  0.001820 byte 81 7C EE 3A 
[ 9, 26, 42] float:  0.001131 byte C8 45 94 3A 
[ 9, 27, 42] float: -0.006043 byte 7B 06 C6 BB 
[ 9, 28, 42] float: -0.003619 byte C0 33 6D BB 
[ 9, 29, 42] float:  0.001774 byte A8 76 E8 3A 
[ 9, 30, 42] float: -0.000053 byte 00 DD 5D B8 
[10,  1, 42] float:  0.005819 byte F6 B0 BE 3B 
[10,  2, 42] float:  0.002848 byte A8 A6 3A 3B 
[10,  3, 42] float: -0.002357 byte DC 75 1A BB 
[10,  4, 42] float: -0.005716 byte DA 4C BB BB 
[10,  5, 42] float: -0.003068 byte C8 08 49 BB 
[10,  6, 42] float:  0.002800 byte 62 82 37 3B 
[10,  7, 42] float:  0.002426 byte F0 04 1F 3B 
[10,  8, 42] float:  0.000424 byte 30 14 DE 39 
[10,  9, 42] float: -0.001390 byte D2 39 B6 BA 
[10, 10, 42] float: -0.001828 byte 6B 90 EF BA 
[10, 11, 42] float:  0.000313 byte 84 FF A3 39 
[10, 12, 42] float:  0.001180 byte 85 A6 9A 3A 
[10, 13, 42] float:  0.002291 byte EE 1C 16 3B 
[10, 14, 42] float:  0.001519 byte F7 08 C7 3A 
[10, 15, 42] float:  0.002657 byte 8B 1B 2E 3B 
[10, 16, 42] float:  0.000923 byte C0 E1 71 3A 
[10, 17, 42] float:  0.002657 byte 8B 1B 2E 3B 
[10, 18, 42] float:  0.001519 byte F7 08 C7 3A 
[10, 19, 42] float:  0.002290 byte 2D 10 16 3B 
[10, 20, 42] float:  0.001178 byte 6A 6E 9A 3A 
[10, 21, 42] float:  0.000271 byte E2 DD 8D 39 
[10, 22, 42] float: -0.001903 byte C7 72 F9 BA 
[10, 23, 42] float: -0.001978 byte 0B A0 01 BB 
[10, 24, 42] float: -0.000337 byte 48 B0 B0 B9 
[10, 25, 42] float:  0.000979 byte F2 5C 80 3A 
[10, 26, 42] float:  0.002313 byte DC 97 17 3B 
[10, 27, 42] float:  0.001338 byte 28 6C AF 3A 
[10, 28, 42] float: -0.000890 byte 5E 35 69 BA 
[10, 29, 42] float: -0.003430 byte 06 D2 60 BB 
[10, 30, 42] float: -0.003967 byte FB FC 81 BB 
[11,  1, 42] float:  0.003576 byte 68 5B 6A 3B 
[11,  2, 42] float:  0.002963 byte BA 28 42 3B 
[11,  3, 42] float: -0.001482 byte CC 40 C2 BA 
[11,  4, 42] float: -0.006672 byte 7C A4 DA BB 
[11,  5, 42] float: -0.003215 byte FA AC 52 BB 
[11,  6, 42] float:  0.003541 byte CD 16 68 3B 
[11,  7, 42] float:  0.000610 byte 12 E0 1F 3A 
[11,  8, 42] float:  0.002039 byte A0 A0 05 3B 
[11,  9, 42] float:  0.001292 byte FC 55 A9 3A 
[11, 10, 42] float:  0.000313 byte 84 FF A3 39 
[11, 11, 42] float:  0.004541 byte 85 CA 94 3B 
[11, 12, 42] float:  0.001976 byte 07 80 01 3B 
[11, 13, 42] float:  0.003447 byte D3 E2 61 3B 
[11, 14, 42] float: -0.000692 byte E0 76 35 BA 
[11, 15, 42] float: -0.000517 byte E0 6E 07 BA 
[11, 16, 42] float: -0.003495 byte 3A 13 65 BB 
[11, 17, 42] float: -0.000517 byte E0 6E 07 BA 
[11, 18, 42] float: -0.000692 byte D0 81 35 BA 
[11, 19, 42] float:  0.003446 byte 94 DC 61 3B 
[11, 20, 42] float:  0.001964 byte 36 AE 00 3B 
[11, 21, 42] float:  0.004517 byte BC 01 94 3B 
[11, 22, 42] float:  0.000056 byte 50 F8 6B 38 
[11, 23, 42] float:  0.000914 byte A9 99 6F 3A 
[11, 24, 42] float:  0.000696 byte B4 85 36 3A 
[11, 25, 42] float: -0.000528 byte B6 69 0A BA 
[11, 26, 42] float:  0.004811 byte DB A2 9D 3B 
[11, 27, 42] float: -0.000108 byte 00 E6 E1 B8 
[11, 28, 42] float: -0.002189 byte DD 77 0F BB 
[11, 29, 42] float: -0.002341 byte A8 72 19 BB 
[11, 30, 42] float: -0.006074 byte FF 09 C7 BB 
[12,  1, 42] float:  0.001241 byte 64 A4 A2 3A 
[12,  2, 42] float:  0.000535 byte 28 40 0C 3A 
[12,  3, 42] float: -0.003275 byte C3 A3 56 BB 
[12,  4, 42] float: -0.004352 byte F0 98 8E BB 
[12,  5, 42] float:  0.002687 byte D4 15 30 3B 
[12,  6, 42] float:  0.003740 byte CE 15 75 3B 
[12,  7, 42] float:  0.000033 byte 00 17 09 38 
[12,  8, 42] float:  0.000514 byte F4 BA 06 3A 
[12,  9, 42] float: -0.002165 byte 23 E4 0D BB 
[12, 10, 42] float:  0.001180 byte 85 A6 9A 3A 
[12, 11, 42] float:  0.001976 byte 07 80 01 3B 
[12, 12, 42] float:  0.000298 byte D0 09 9C 39 
[12, 13, 42] float: -0.003226 byte 2E 6E 53 BB 
[12, 14, 42] float: -0.008309 byte 52 24 08 BC 
[12, 15, 42] float: -0.010491 byte 6A E0 2B BC 
[12, 16, 42] float: -0.012121 byte C9 95 46 BC 
[12, 17, 42] float: -0.010491 byte 88 E0 2B BC 
[12, 18, 42] float: -0.008310 byte 99 24 08 BC 
[12, 19, 42] float: -0.003229 byte 4E A1 53 BB 
[12, 20, 42] float:  0.000291 byte 88 C8 98 39 
[12, 21, 42] float:  0.001885 byte EA 0C F7 3A 
[12, 22, 42] float:  0.001030 byte 71 0E 87 3A 
[12, 23, 42] float: -0.002993 byte 44 25 44 BB 
[12, 24, 42] float: -0.000426 byte 02 79 DF B9 
[12, 25, 42] float: -0.000822 byte 22 63 57 BA 
[12, 26, 42] float:  0.004285 byte 63 68 8C 3B 
[12, 27, 42] float:  0.007964 byte 06 7B 02 3C 
[12, 28, 42] float: -0.000535 byte 08 45 0C BA 
[12, 29, 42] float: -0.008144 byte 4C 70 05 BC 
[12, 30, 42] float: -0.007147 byte F9 33 EA BB 
[13,  1, 42] float: -0.000088 byte 00 81 B7 B8 
[13,  2, 42] float: -0.000780 byte 58 94 4C BA 
[13,  3, 42] float: -0.001018 byte 59 6D 85 BA 
[13,  4, 42] float: -0.001787 byte EC 34 EA BA 
[13,  5, 42] float: -0.000019 byte 80 D8 9E B7 
[13,  6, 42] float:  0.001531 byte DE A2 C8 3A 
[13,  7, 42] float: -0.001477 byte AF A1 C1 BA 
[13,  8, 42] float:  0.001252 byte 44 0E A4 3A 
[13,  9, 42] float:  0.002086 byte BB BB 08 3B 
[13, 10, 42] float:  0.002291 byte EE 1C 16 3B 
[13, 11, 42] float:  0.003447 byte D3 E2 61 3B 
[13, 12, 42] float: -0.003226 byte 2E 6E 53 BB 
[13, 13, 42] float: -0.008386 byte 93 66 09 BC 
[13, 14, 42] float: -0.012688 byte B0 E0 4F BC 
[13, 15, 42] float: -0.012396 byte 66 18 4B BC 
[13, 16, 42] float: -0.012707 byte 9A 2F 50 BC 
[13, 17, 42] float: -0.012396 byte 6F 18 4B BC 
[13, 18, 42] float: -0.012688 byte 2C E3 4F BC 
[13, 19, 42] float: -0.008388 byte F4 6B 09 BC 
[13, 20, 42] float: -0.003252 byte E8 23 55 BB 
[13, 21, 42] float:  0.003400 byte 9E CF 5E 3B 
[13, 22, 42] float:  0.001910 byte B9 5F FA 3A 
[13, 23, 42] float:  0.001569 byte 96 A7 CD 3A 
[13, 24, 42] float: -0.000108 byte 60 25 E3 B8 
[13, 25, 42] float: -0.002349 byte 06 EA 19 BB 
[13, 26, 42] float:  0.004006 byte A8 41 83 3B 
[13, 27, 42] float:  0.003785 byte 14 0D 78 3B 
[13, 28, 42] float:  0.000575 byte C2 A7 16 3A 
[13, 29, 42] float: -0.004467 byte 3B 60 92 BB 
[13, 30, 42] float: -0.009058 byte 9F 69 14 BC 
[14,  1, 42] float: -0.001965 byte EC C4 00 BB 
[14,  2, 42] float: -0.000957 byte C4 CA 7A BA 
[14,  3, 42] float: -0.002641 byte 53 10 2D BB 
[14,  4, 42] float: -0.002585 byte FE 61 29 BB 
[14,  5, 42] float:  0.004597 byte 53 9F 96 3B 
[14,  6, 42] float:  0.002798 byte C3 64 37 3B 
[14,  7, 42] float: -0.000532 byte 7E 8C 0B BA 
[14,  8, 42] float:  0.001924 byte F6 2A FC 3A 
[14,  9, 42] float: -0.002155 byte 1E 40 0D BB 
[14, 10, 42] float:  0.001519 byte F7 08 C7 3A 
[14, 11, 42] float: -0.000692 byte E0 76 35 BA 
[14, 12, 42] float: -0.008309 byte 52 24 08 BC 
[14, 13, 42] float: -0.012688 byte B0 E0 4F BC 
[14, 14, 42] float: -0.012249 byte DA AE 48 BC 
[14, 15, 42] float: -0.007607 byte A6 40 F9 BB 
[14, 16, 42] float: -0.008300 byte 0D FE 07 BC 
[14, 17, 42] float: -0.007607 byte 5E 41 F9 BB 
[14, 18, 42] float: -0.012249 byte B3 AF 48 BC 
[14, 19, 42] float: -0.012694 byte 59 F9 4F BC 
[14, 20, 42] float: -0.008321 byte 7F 54 08 BC 
[14, 21, 42] float: -0.000826 byte B8 8E 58 BA 
[14, 22, 42] float:  0.001310 byte 5B AE AB 3A 
[14, 23, 42] float: -0.003115 byte 8F 1F 4C BB 
[14, 24, 42] float:  0.000930 byte BA AF 73 3A 
[14, 25, 42] float: -0.000857 byte 25 C1 60 BA 
[14, 26, 42] float:  0.004031 byte 53 13 84 3B 
[14, 27, 42] float:  0.009892 byte D6 13 22 3C 
[14, 28, 42] float:  0.000095 byte 00 AC C6 38 
[14, 29, 42] float: -0.009322 byte 00 BB 18 BC 
[14, 30, 42] float: -0.008055 byte 25 F7 03 BC 
[15,  1, 42] float: -0.001969 byte 34 02 01 BB 
[15,  2, 42] float: -0.002575 byte 5A BB 28 BB 
[15,  3, 42] float: -0.000581 byte 50 5A 18 BA 
[15,  4, 42] float:  0.000880 byte 20 B3 66 3A 
[15,  5, 42] float:  0.001245 byte D5 35 A3 3A 
[15,  6, 42] float:  0.000198 byte 00 9F 4F 39 
[15,  7, 42] float: -0.001895 byte C5 66 F8 BA 
[15,  8, 42] float:  0.000948 byte 8E 6E 78 3A 
[15,  9, 42] float:  0.002024 byte DA AC 04 3B 
[15, 10, 42] float:  0.002657 byte 8B 1B 2E 3B 
[15, 11, 42] float: -0.000517 byte E0 6E 07 BA 
[15, 12, 42] float: -0.010491 byte 6A E0 2B BC 
[15, 13, 42] float: -0.012396 byte 66 18 4B BC 
[15, 14, 42] float: -0.007607 byte A6 40 F9 BB 
[15, 15, 42] float:  0.005117 byte 40 AA A7 3B 
[15, 16, 42] float:  0.039540 byte F4 F4 21 3D 
[15, 17, 42] float:  0.005117 byte F8 A9 A7 3B 
[15, 18, 42] float: -0.007608 byte E4 48 F9 BB 
[15, 19, 42] float: -0.012398 byte 49 21 4B BC 
[15, 20, 42] float: -0.010526 byte 3E 77 2C BC 
[15, 21, 42] float: -0.000579 byte 48 E2 17 BA 
[15, 22, 42] float:  0.002207 byte 8D 9F 10 3B 
[15, 23, 42] float:  0.001437 byte AA 67 BC 3A 
[15, 24, 42] float: -0.000362 byte 50 C9 BD B9 
[15, 25, 42] float: -0.002563 byte 8F FB 27 BB 
[15, 26, 42] float:  0.003229 byte 43 A5 53 3B 
[15, 27, 42] float:  0.005214 byte 3A D6 AA 3B 
[15, 28, 42] float:  0.001981 byte DF CD 01 3B 
[15, 29, 42] float: -0.005138 byte 13 5B A8 BB 
[15, 30, 42] float: -0.009883 byte 3C ED 21 BC 
[16,  1, 42] float: -0.003033 byte BE C6 46 BB 
[16,  2, 42] float: -0.001430 byte 24 6B BB BA 
[16,  3, 42] float: -0.002244 byte 0D 16 13 BB 
[16,  4, 42] float: -0.001910 byte 88 56 FA BA 
[16,  5, 42] float:  0.004918 byte A6 26 A1 3B 
[16,  6, 42] float:  0.002358 byte 73 84 1A 3B 
[16,  7, 42] float: -0.000511 byte A2 DB 05 BA 
[16,  8, 42] float:  0.002457 byte C4 06 21 3B 
[16,  9, 42] float: -0.002147 byte 3C AE 0C BB 
[16, 10, 42] float:  0.000923 byte C0 E1 71 3A 
[16, 11, 42] float: -0.003495 byte 3A 13 65 BB 
[16, 12, 42] float: -0.012121 byte C9 95 46 BC 
[16, 13, 42] float: -0.012707 byte 9A 2F 50 BC 
[16, 14, 42] float: -0.008300 byte 0D FE 07 BC 
[16, 15, 42] float:  0.039540 byte F4 F4 21 3D 
[16, 16, 42] float:  0.129409 byte E1 83 04 3E 
[16, 17, 42] float:  0.039540 byte D9 F4 21 3D 
[16, 18, 42] float: -0.008301 byte 99 FF 07 BC 
[16, 19, 42] float: -0.012714 byte A2 4D 50 BC 
[16, 20, 42] float: -0.012134 byte BE CF 46 BC 
[16, 21, 42] float: -0.003646 byte 7C F0 6E BB 
[16, 22, 42] float:  0.000692 byte 8E 66 35 3A 
[16, 23, 42] float: -0.003145 byte E0 19 4E BB 
[16, 24, 42] float:  0.001457 byte 95 F3 BE 3A 
[16, 25, 42] float: -0.000635 byte 20 7A 26 BA 
[16, 26, 42] float:  0.003818 byte 95 37 7A 3B 
[16, 27, 42] float:  0.010128 byte 54 EF 25 3C 
[16, 28, 42] float:  0.000321 byte C8 65 A8 39 
[16, 29, 42] float: -0.009408 byte 8E 22 1A BC 
[16, 30, 42] float: -0.008156 byte 1A A2 05 BC 
[17,  1, 42] float: -0.001969 byte 34 02 01 BB 
[17,  2, 42] float: -0.002575 byte 5A BB 28 BB 
[17,  3, 42] float: -0.000581 byte 50 5A 18 BA 
[17,  4, 42] float:  0.000880 byte 20 B3 66 3A 
[17,  5, 42] float:  0.001245 byte D5 35 A3 3A 
[17,  6, 42] float:  0.000198 byte 00 9F 4F 39 
[17,  7, 42] float: -0.001895 byte C5 66 F8 BA 
[17,  8, 42] float:  0.000948 byte 8E 6E 78 3A 
[17,  9, 42] float:  0.002024 byte DA AC 04 3B 
[17, 10, 42] float:  0.002657 byte 8B 1B 2E 3B 
[17, 11, 42] float: -0.000517 byte E0 6E 07 BA 
[17, 12, 42] float: -0.010491 byte 88 E0 2B BC 
[17, 13, 42] float: -0.012396 byte 6F 18 4B BC 
[17, 14, 42] float: -0.007607 byte 5E 41 F9 BB 
[17, 15, 42] float:  0.005117 byte F8 A9 A7 3B 
[17, 16, 42] float:  0.039540 byte D9 F4 21 3D 
[17, 17, 42] float:  0.005117 byte B4 A9 A7 3B 
[17, 18, 42] float: -0.007608 byte A2 49 F9 BB 
[17, 19, 42] float: -0.012398 byte 4F 21 4B BC 
[17, 20, 42] float: -0.010527 byte 5E 77 2C BC 
[17, 21, 42] float: -0.000579 byte 48 E2 17 BA 
[17, 22, 42] float:  0.002207 byte 8D 9F 10 3B 
[17, 23, 42] float:  0.001437 byte AA 67 BC 3A 
[17, 24, 42] float: -0.000362 byte 50 C9 BD B9 
[17, 25, 42] float: -0.002563 byte 8F FB 27 BB 
[17, 26, 42] float:  0.003229 byte 43 A5 53 3B 
[17, 27, 42] float:  0.005214 byte 3A D6 AA 3B 
[17, 28, 42] float:  0.001981 byte DF CD 01 3B 
[17, 29, 42] float: -0.005138 byte 13 5B A8 BB 
[17, 30, 42] float: -0.009883 byte 3C ED 21 BC 
[18,  1, 42] float: -0.001965 byte EC C4 00 BB 
[18,  2, 42] float: -0.000957 byte C4 CA 7A BA 
[18,  3, 42] float: -0.002641 byte 53 10 2D BB 
[18,  4, 42] float: -0.002585 byte FE 61 29 BB 
[18,  5, 42] float:  0.004597 byte 53 9F 96 3B 
[18,  6, 42] float:  0.002798 byte C3 64 37 3B 
[18,  7, 42] float: -0.000532 byte 7E 8C 0B BA 
[18,  8, 42] float:  0.001924 byte F6 2A FC 3A 
[18,  9, 42] float: -0.002155 byte 1E 40 0D BB 
[18, 10, 42] float:  0.001519 byte F7 08 C7 3A 
[18, 11, 42] float: -0.000692 byte D0 81 35 BA 
[18, 12, 42] float: -0.008310 byte 99 24 08 BC 
[18, 13, 42] float: -0.012688 byte 2C E3 4F BC 
[18, 14, 42] float: -0.012249 byte B3 AF 48 BC 
[18, 15, 42] float: -0.007608 byte E4 48 F9 BB 
[18, 16, 42] float: -0.008301 byte 99 FF 07 BC 
[18, 17, 42] float: -0.007608 byte A2 49 F9 BB 
[18, 18, 42] float: -0.012249 byte 8E B0 48 BC 
[18, 19, 42] float: -0.012694 byte D8 FB 4F BC 
[18, 20, 42] float: -0.008321 byte C4 54 08 BC 
[18, 21, 42] float: -0.000826 byte A0 99 58 BA 
[18, 22, 42] float:  0.001310 byte 5B AE AB 3A 
[18, 23, 42] float: -0.003115 byte 8F 1F 4C BB 
[18, 24, 42] float:  0.000930 byte BA AF 73 3A 
[18, 25, 42] float: -0.000857 byte 25 C1 60 BA 
[18, 26, 42] float:  0.004031 byte 53 13 84 3B 
[18, 27, 42] float:  0.009892 byte D6 13 22 3C 
[18, 28, 42] float:  0.000095 byte 00 AC C6 38 
[18, 29, 42] float: -0.009322 byte 00 BB 18 BC 
[18, 30, 42] float: -0.008055 byte 25 F7 03 BC 
[19,  1, 42] float: -0.000088 byte 00 81 B7 B8 
[19,  2, 42] float: -0.000780 byte 58 94 4C BA 
[19,  3, 42] float: -0.001018 byte 59 6D 85 BA 
[19,  4, 42] float: -0.001787 byte EC 34 EA BA 
[19,  5, 42] float: -0.000019 byte 80 D8 9E B7 
[19,  6, 42] float:  0.001531 byte DE A2 C8 3A 
[19,  7, 42] float: -0.001477 byte AF A1 C1 BA 
[19,  8, 42] float:  0.001252 byte 44 0E A4 3A 
[19,  9, 42] float:  0.002086 byte BB BB 08 3B 
[19, 10, 42] float:  0.002290 byte 2D 10 16 3B 
[19, 11, 42] float:  0.003446 byte 94 DC 61 3B 
[19, 12, 42] float: -0.003229 byte 4E A1 53 BB 
[19, 13, 42] float: -0.008388 byte F4 6B 09 BC 
[19, 14, 42] float: -0.012694 byte 59 F9 4F BC 
[19, 15, 42] float: -0.012398 byte 49 21 4B BC 
[19, 16, 42] float: -0.012714 byte A2 4D 50 BC 
[19, 17, 42] float: -0.012398 byte 4F 21 4B BC 
[19, 18, 42] float: -0.012694 byte D8 FB 4F BC 
[19, 19, 42] float: -0.008389 byte 53 71 09 BC 
[19, 20, 42] float: -0.003255 byte 06 57 55 BB 
[19, 21, 42] float:  0.003399 byte 5F C9 5E 3B 
[19, 22, 42] float:  0.001909 byte 3E 46 FA 3A 
[19, 23, 42] float:  0.001569 byte 96 A7 CD 3A 
[19, 24, 42] float: -0.000108 byte 60 25 E3 B8 
[19, 25, 42] float: -0.002349 byte 06 EA 19 BB 
[19, 26, 42] float:  0.004006 byte A8 41 83 3B 
[19, 27, 42] float:  0.003785 byte 14 0D 78 3B 
[19, 28, 42] float:  0.000575 byte C2 A7 16 3A 
[19, 29, 42] float: -0.004467 byte 3B 60 92 BB 
[19, 30, 42] float: -0.009058 byte 9F 69 14 BC 
[20,  1, 42] float:  0.001241 byte 64 A4 A2 3A 
[20,  2, 42] float:  0.000535 byte 28 40 0C 3A 
[20,  3, 42] float: -0.003275 byte C3 A3 56 BB 
[20,  4, 42] float: -0.004352 byte F0 98 8E BB 
[20,  5, 42] float:  0.002687 byte D4 15 30 3B 
[20,  6, 42] float:  0.003740 byte CE 15 75 3B 
[20,  7, 42] float:  0.000033 byte 00 17 09 38 
[20,  8, 42] float:  0.000514 byte F4 BA 06 3A 
[20,  9, 42] float: -0.002168 byte 51 15 0E BB 
[20, 10, 42] float:  0.001178 byte 6A 6E 9A 3A 
[20, 11, 42] float:  0.001964 byte 36 AE 00 3B 
[20, 12, 42] float:  0.000291 byte 88 C8 98 39 
[20, 13, 42] float: -0.003252 byte E8 23 55 BB 
[20, 14, 42] float: -0.008321 byte 7F 54 08 BC 
[20, 15, 42] float: -0.010526 byte 3E 77 2C BC 
[20, 16, 42] float: -0.012134 byte BE CF 46 BC 
[20, 17, 42] float: -0.010527 byte 5E 77 2C BC 
[20, 18, 42] float: -0.008321 byte C4 54 08 BC 
[20, 19, 42] float: -0.003255 byte 06 57 55 BB 
[20, 20, 42] float:  0.000285 byte 08 87 95 39 
[20, 21, 42] float:  0.001872 byte 54 69 F5 3A 
[20, 22, 42] float:  0.001029 byte 44 D6 86 3A 
[20, 23, 42] float: -0.002996 byte 70 56 44 BB 
[20, 24, 42] float: -0.000426 byte 02 79 DF B9 
[20, 25, 42] float: -0.000822 byte 22 63 57 BA 
[20, 26, 42] float:  0.004285 byte 63 68 8C 3B 
[20, 27, 42] float:  0.007964 byte 06 7B 02 3C 
[20, 28, 42] float: -0.000535 byte 08 45 0C BA 
[20, 29, 42] float: -0.008144 byte 4C 70 05 BC 
[20, 30, 42] float: -0.007147 byte F9 33 EA BB 
[21,  1, 42] float:  0.003576 byte 68 5B 6A 3B 
[21,  2, 42] float:  0.002963 byte BA 28 42 3B 
[21,  3, 42] float: -0.001482 byte CC 40 C2 BA 
[21,  4, 42] float: -0.006672 byte 7C A4 DA BB 
[21,  5, 42] float: -0.003215 byte FA AC 52 BB 
[21,  6, 42] float:  0.003541 byte CD 16 68 3B 
[21,  7, 42] float:  0.000610 byte 12 E0 1F 3A 
[21,  8, 42] float:  0.002029 byte CD 00 05 3B 
[21,  9, 42] float:  0.001286 byte 7E 85 A8 3A 
[21, 10, 42] float:  0.000271 byte E2 DD 8D 39 
[21, 11, 42] float:  0.004517 byte BC 01 94 3B 
[21, 12, 42] float:  0.001885 byte EA 0C F7 3A 
[21, 13, 42] float:  0.003400 byte 9E CF 5E 3B 
[21, 14, 42] float: -0.000826 byte B8 8E 58 BA 
[21, 15, 42] float: -0.000579 byte 48 E2 17 BA 
[21, 16, 42] float: -0.003646 byte 7C F0 6E BB 
[21, 17, 42] float: -0.000579 byte 48 E2 17 BA 
[21, 18, 42] float: -0.000826 byte A0 99 58 BA 
[21, 19, 42] float:  0.003399 byte 5F C9 5E 3B 
[21, 20, 42] float:  0.001872 byte 54 69 F5 3A 
[21, 21, 42] float:  0.004493 byte EF 38 93 3B 
[21, 22, 42] float:  0.000014 byte 40 B2 6B 37 
[21, 23, 42] float:  0.000908 byte 91 F8 6D 3A 
[21, 24, 42] float:  0.000687 byte 6C 06 34 3A 
[21, 25, 42] float: -0.000528 byte B6 69 0A BA 
[21, 26, 42] float:  0.004811 byte DB A2 9D 3B 
[21, 27, 42] float: -0.000108 byte 00 E6 E1 B8 
[21, 28, 42] float: -0.002189 byte DD 77 0F BB 
[21, 29, 42] float: -0.002341 byte A8 72 19 BB 
[21, 30, 42] float: -0.006074 byte FF 09 C7 BB 
[22,  1, 42] float:  0.005819 byte F6 B0 BE 3B 
[22,  2, 42] float:  0.002848 byte A8 A6 3A 3B 
[22,  3, 42] float: -0.002357 byte DC 75 1A BB 
[22,  4, 42] float: -0.005716 byte DA 4C BB BB 
[22,  5, 42] float: -0.003068 byte C8 08 49 BB 
[22,  6, 42] float:  0.002800 byte 62 82 37 3B 
[22,  7, 42] float:  0.002400 byte 02 49 1D 3B 
[22,  8, 42] float:  0.000404 byte 70 E5 D3 39 
[22,  9, 42] float: -0.001510 byte 3E E3 C5 BA 
[22, 10, 42] float: -0.001903 byte C7 72 F9 BA 
[22, 11, 42] float:  0.000056 byte 50 F8 6B 38 
[22, 12, 42] float:  0.001030 byte 71 0E 87 3A 
[22, 13, 42] float:  0.001910 byte B9 5F FA 3A 
[22, 14, 42] float:  0.001310 byte 5B AE AB 3A 
[22, 15, 42] float:  0.002207 byte 8D 9F 10 3B 
[22, 16, 42] float:  0.000692 byte 8E 66 35 3A 
[22, 17, 42] float:  0.002207 byte 8D 9F 10 3B 
[22, 18, 42] float:  0.001310 byte 5B AE AB 3A 
[22, 19, 42] float:  0.001909 byte 3E 46 FA 3A 
[22, 20, 42] float:  0.001029 byte 44 D6 86 3A 
[22, 21, 42] float:  0.000014 byte 40 B2 6B 37 
[22, 22, 42] float: -0.001979 byte 95 AA 01 BB 
[22, 23, 42] float: -0.002097 byte C0 74 09 BB 
[22, 24, 42] float: -0.000356 byte 18 DF BA B9 
[22, 25, 42] float:  0.000953 byte 2C CA 79 3A 
[22, 26, 42] float:  0.002313 byte DC 97 17 3B 
[22, 27, 42] float:  0.001338 byte 28 6C AF 3A 
[22, 28, 42] float: -0.000890 byte 5E 35 69 BA 
[22, 29, 42] float: -0.003430 byte 06 D2 60 BB 
[22, 30, 42] float: -0.003967 byte FB FC 81 BB 
[23,  1, 42] float:  0.007525 byte 9B 93 F6 3B 
[23,  2, 42] float:  0.007195 byte 0C C8 EB 3B 
[23,  3, 42] float: -0.001269 byte 7C 64 A6 BA 
[23,  4, 42] float: -0.009777 byte CC 2F 20 BC 
[23,  5, 42] float: -0.007493 byte EA 8B F5 BB 
[23,  6, 42] float:  0.001509 byte B8 D6 C5 3A 
[23,  7, 42] float:  0.002965 byte D2 4D 42 3B 
[23,  8, 42] float:  0.005233 byte BA 76 AB 3B 
[23,  9, 42] float:  0.003123 byte 7A A7 4C 3B 
[23, 10, 42] float: -0.001978 byte 0B A0 01 BB 
[23, 11, 42] float:  0.000914 byte A9 99 6F 3A 
[23, 12, 42] float: -0.002993 byte 44 25 44 BB 
[23, 13, 42] float:  0.001569 byte 96 A7 CD 3A 
[23, 14, 42] float: -0.003115 byte 8F 1F 4C BB 
[23, 15, 42] float:  0.001437 byte AA 67 BC 3A 
[23, 16, 42] float: -0.003145 byte E0 19 4E BB 
[23, 17, 42] float:  0.001437 byte AA 67 BC 3A 
[23, 18, 42] float: -0.003115 byte 8F 1F 4C BB 
[23, 19, 42] float:  0.001569 byte 96 A7 CD 3A 
[23, 20, 42] float: -0.002996 byte 70 56 44 BB 
[23, 21, 42] float:  0.000908 byte 91 F8 6D 3A 
[23, 22, 42] float: -0.002097 byte C0 74 09 BB 
[23, 23, 42] float:  0.002925 byte 53 B1 3F 3B 
[23, 24, 42] float:  0.004137 byte 0E 90 87 3B 
[23, 25, 42] float:  0.001768 byte AC B2 E7 3A 
[23, 26, 42] float:  0.001068 byte E8 F2 8B 3A 
[23, 27, 42] float: -0.006043 byte 7B 06 C6 BB 
[23, 28, 42] float: -0.003619 byte C0 33 6D BB 
[23, 29, 42] float:  0.001774 byte A8 76 E8 3A 
[23, 30, 42] float: -0.000053 byte 00 DD 5D B8 
[24,  1, 42] float:  0.008775 byte 19 C3 0F 3C 
[24,  2, 42] float:  0.004492 byte 0C 34 93 3B 
[24,  3, 42] float:  0.001265 byte AC CC A5 3A 
[24,  4, 42] float: -0.003430 byte 2A CD 60 BB 
[24,  5, 42] float: -0.010262 byte A7 22 28 BC 
[24,  6, 42] float: -0.003428 byte 16 AA 60 BB 
[24,  7, 42] float:  0.000313 byte 7F 58 A4 39 
[24,  8, 42] float: -0.000053 byte A0 B7 5D B8 
[24,  9, 42] float:  0.004425 byte E1 FF 90 3B 
[24, 10, 42] float: -0.000337 byte 48 B0 B0 B9 
[24, 11, 42] float:  0.000696 byte B4 85 36 3A 
[24, 12, 42] float: -0.000426 byte 02 79 DF B9 
[24, 13, 42] float: -0.000108 byte 60 25 E3 B8 
[24, 14, 42] float:  0.000930 byte BA AF 73 3A 
[24, 15, 42] float: -0.000362 byte 50 C9 BD B9 
[24, 16, 42] float:  0.001457 byte 95 F3 BE 3A 
[24, 17, 42] float: -0.000362 byte 50 C9 BD B9 
[24, 18, 42] float:  0.000930 byte BA AF 73 3A 
[24, 19, 42] float: -0.000108 byte 60 25 E3 B8 
[24, 20, 42] float: -0.000426 byte 02 79 DF B9 
[24, 21, 42] float:  0.000687 byte 6C 06 34 3A 
[24, 22, 42] float: -0.000356 byte 18 DF BA B9 
[24, 23, 42] float:  0.004137 byte 0E 90 87 3B 
[24, 24, 42] float: -0.000491 byte C8 9D 00 BA 
[24, 25, 42] float: -0.001320 byte BB 0C AD BA 
[24, 26, 42] float: -0.004802 byte 98 59 9D BB 
[24, 27, 42] float: -0.008260 byte 1C 55 07 BC 
[24, 28, 42] float:  0.000726 byte AA 56 3E 3A 
[24, 29, 42] float:  0.005405 byte A8 1F B1 3B 
[24, 30, 42] float:  0.002110 byte 2A 41 0A 3B 
[25,  1, 42] float:  0.007539 byte B4 0D F7 3B 
[25,  2, 42] float:  0.007154 byte 5A 68 EA 3B 
[25,  3, 42] float: -0.000721 byte F8 E3 3C BA 
[25,  4, 42] float: -0.004277 byte F9 29 8C BB 
[25,  5, 42] float: -0.006353 byte 9A 2E D0 BB 
[25,  6, 42] float: -0.006948 byte CF AF E3 BB 
[25,  7, 42] float: -0.000575 byte 70 C8 16 BA 
[25,  8, 42] float: -0.000722 byte 90 44 3D BA 
[25,  9, 42] float:  0.001820 byte 81 7C EE 3A 
[25, 10, 42] float:  0.000979 byte F2 5C 80 3A 
[25, 11, 42] float: -0.000528 byte B6 69 0A BA 
[25, 12, 42] float: -0.000822 byte 22 63 57 BA 
[25, 13, 42] float: -0.002349 byte 06 EA 19 BB 
[25, 14, 42] float: -0.000857 byte 25 C1 60 BA 
[25, 15, 42] float: -0.002563 byte 8F FB 27 BB 
[25, 16, 42] float: -0.000635 byte 20 7A 26 BA 
[25, 17, 42] float: -0.002563 byte 8F FB 27 BB 
[25, 18, 42] float: -0.000857 byte 25 C1 60 BA 
[25, 19, 42] float: -0.002349 byte 06 EA 19 BB 
[25, 20, 42] float: -0.000822 byte 22 63 57 BA 
[25, 21, 42] float: -0.000528 byte B6 69 0A BA 
[25, 22, 42] float:  0.000953 byte 2C CA 79 3A 
[25, 23, 42] float:  0.001768 byte AC B2 E7 3A 
[25, 24, 42] float: -0.001320 byte BB 0C AD BA 
[25, 25, 42] float: -0.001402 byte EC C2 B7 BA 
[25, 26, 42] float: -0.008784 byte F4 E8 0F BC 
[25, 27, 42] float: -0.007170 byte 61 F6 EA BB 
[25, 28, 42] float:  0.000857 byte E2 A6 60 3A 
[25, 29, 42] float:  0.005186 byte 78 EB A9 3B 
[25, 30, 42] float:  0.006048 byte BF 2B C6 3B 
[26,  1, 42] float:  0.005844 byte 9E 7B BF 3B 
[26,  2, 42] float:  0.003035 byte AD EE 46 3B 
[26,  3, 42] float:  0.002407 byte 82 B7 1D 3B 
[26,  4, 42] float:  0.001219 byte C0 D1 9F 3A 
[26,  5, 42] float: -0.007946 byte F8 2D 02 BC 
[26,  6, 42] float: -0.009517 byte F1 EB 1B BC 
[26,  7, 42] float: -0.007701 byte 42 57 FC BB 
[26,  8, 42] float: -0.004683 byte 59 72 99 BB 
[26,  9, 42] float:  0.001131 byte C8 45 94 3A 
[26, 10, 42] float:  0.002313 byte DC 97 17 3B 
[26, 11, 42] float:  0.004811 byte DB A2 9D 3B 
[26, 12, 42] float:  0.004285 byte 63 68 8C 3B 
[26, 13, 42] float:  0.004006 byte A8 41 83 3B 
[26, 14, 42] float:  0.004031 byte 53 13 84 3B 
[26, 15, 42] float:  0.003229 byte 43 A5 53 3B 
[26, 16, 42] float:  0.003818 byte 95 37 7A 3B 
[26, 17, 42] float:  0.003229 byte 43 A5 53 3B 
[26, 18, 42] float:  0.004031 byte 53 13 84 3B 
[26, 19, 42] float:  0.004006 byte A8 41 83 3B 
[26, 20, 42] float:  0.004285 byte 63 68 8C 3B 
[26, 21, 42] float:  0.004811 byte DB A2 9D 3B 
[26, 22, 42] float:  0.002313 byte DC 97 17 3B 
[26, 23, 42] float:  0.001068 byte E8 F2 8B 3A 
[26, 24, 42] float: -0.004802 byte 98 59 9D BB 
[26, 25, 42] float: -0.008784 byte F4 E8 0F BC 
[26, 26, 42] float: -0.010862 byte 21 F7 31 BC 
[26, 27, 42] float: -0.009156 byte 3E 02 16 BC 
[26, 28, 42] float:  0.002068 byte 4E 84 07 3B 
[26, 29, 42] float:  0.009575 byte 6B E0 1C 3C 
[26, 30, 42] float:  0.007807 byte B4 D5 FF 3B 
[27,  1, 42] float: -0.000398 byte A8 B4 D0 B9 
[27,  2, 42] float: -0.000232 byte 70 81 73 B9 
[27,  3, 42] float: -0.001454 byte 2A 97 BE BA 
[27,  4, 42] float:  0.002117 byte 90 B5 0A 3B 
[27,  5, 42] float:  0.002448 byte 6D 72 20 3B 
[27,  6, 42] float: -0.007432 byte 44 84 F3 BB 
[27,  7, 42] float: -0.006932 byte E6 27 E3 BB 
[27,  8, 42] float: -0.008127 byte 2C 28 05 BC 
[27,  9, 42] float: -0.006043 byte 7B 06 C6 BB 
[27, 10, 42] float:  0.001338 byte 28 6C AF 3A 
[27, 11, 42] float: -0.000108 byte 00 E6 E1 B8 
[27, 12, 42] float:  0.007964 byte 06 7B 02 3C 
[27, 13, 42] float:  0.003785 byte 14 0D 78 3B 
[27, 14, 42] float:  0.009892 byte D6 13 22 3C 
[27, 15, 42] float:  0.005214 byte 3A D6 AA 3B 
[27, 16, 42] float:  0.010128 byte 54 EF 25 3C 
[27, 17, 42] float:  0.005214 byte 3A D6 AA 3B 
[27, 18, 42] float:  0.009892 byte D6 13 22 3C 
[27, 19, 42] float:  0.003785 byte 14 0D 78 3B 
[27, 20, 42] float:  0.007964 byte 06 7B 02 3C 
[27, 21, 42] float: -0.000108 byte 00 E6 E1 B8 
[27, 22, 42] float:  0.001338 byte 28 6C AF 3A 
[27, 23, 42] float: -0.006043 byte 7B 06 C6 BB 
[27, 24, 42] float: -0.008260 byte 1C 55 07 BC 
[27, 25, 42] float: -0.007170 byte 61 F6 EA BB 
[27, 26, 42] float: -0.009156 byte 3E 02 16 BC 
[27, 27, 42] float:  0.000542 byte B4 2D 0E 3A 
[27, 28, 42] float:  0.002677 byte D8 6F 2F 3B 
[27, 29, 42] float:  0.002113 byte 54 76 0A 3B 
[27, 30, 42] float:  0.006289 byte CE 14 CE 3B 
[28,  1, 42] float: -0.004084 byte 7B D4 85 BB 
[28,  2, 42] float: -0.002433 byte 90 6B 1F BB 
[28,  3, 42] float: -0.005225 byte 09 33 AB BB 
[28,  4, 42] float: -0.003360 byte 98 2E 5C BB 
[28,  5, 42] float:  0.005118 byte A6 B1 A7 3B 
[28,  6, 42] float:  0.002485 byte 05 D7 22 3B 
[28,  7, 42] float:  0.001100 byte DF 3B 90 3A 
[28,  8, 42] float:  0.000726 byte AA 56 3E 3A 
[28,  9, 42] float: -0.003619 byte C0 33 6D BB 
[28, 10, 42] float: -0.000890 byte 5E 35 69 BA 
[28, 11, 42] float: -0.002189 byte DD 77 0F BB 
[28, 12, 42] float: -0.000535 byte 08 45 0C BA 
[28, 13, 42] float:  0.000575 byte C2 A7 16 3A 
[28, 14, 42] float:  0.000095 byte 00 AC C6 38 
[28, 15, 42] float:  0.001981 byte DF CD 01 3B 
[28, 16, 42] float:  0.000321 byte C8 65 A8 39 
[28, 17, 42] float:  0.001981 byte DF CD 01 3B 
[28, 18, 42] float:  0.000095 byte 00 AC C6 38 
[28, 19, 42] float:  0.000575 byte C2 A7 16 3A 
[28, 20, 42] float: -0.000535 byte 08 45 0C BA 
[28, 21, 42] float: -0.002189 byte DD 77 0F BB 
[28, 22, 42] float: -0.000890 byte 5E 35 69 BA 
[28, 23, 42] float: -0.003619 byte C0 33 6D BB 
[28, 24, 42] float:  0.000726 byte AA 56 3E 3A 
[28, 25, 42] float:  0.000857 byte E2 A6 60 3A 
[28, 26, 42] float:  0.002068 byte 4E 84 07 3B 
[28, 27, 42] float:  0.002677 byte D8 6F 2F 3B 
[28, 28, 42] float: -0.005740 byte C8 18 BC BB 
[28, 29, 42] float: -0.001930 byte DB 07 FD BA 
[28, 30, 42] float:  0.004272 byte 1A F8 8B 3B 
[29,  1, 42] float: -0.010311 byte 6C F0 28 BC 
[29,  2, 42] float: -0.010233 byte 65 AA 27 BC 
[29,  3, 42] float: -0.001318 byte 64 CA AC BA 
[29,  4, 42] float:  0.001172 byte E6 AC 99 3A 
[29,  5, 42] float:  0.002754 byte 6A 7F 34 3B 
[29,  6, 42] float:  0.009968 byte CE 51 23 3C 
[29,  7, 42] float:  0.005186 byte 78 EB A9 3B 
[29,  8, 42] float:  0.005405 byte A8 1F B1 3B 
[29,  9, 42] float:  0.001774 byte A8 76 E8 3A 
[29, 10, 42] float: -0.003430 byte 06 D2 60 BB 
[29, 11, 42] float: -0.002341 byte A8 72 19 BB 
[29, 12, 42] float: -0.008144 byte 4C 70 05 BC 
[29, 13, 42] float: -0.004467 byte 3B 60 92 BB 
[29, 14, 42] float: -0.009322 byte 00 BB 18 BC 
[29, 15, 42] float: -0.005138 byte 13 5B A8 BB 
[29, 16, 42] float: -0.009408 byte 8E 22 1A BC 
[29, 17, 42] float: -0.005138 byte 13 5B A8 BB 
[29, 18, 42] float: -0.009322 byte 00 BB 18 BC 
[29, 19, 42] float: -0.004467 byte 3B 60 92 BB 
[29, 20, 42] float: -0.008144 byte 4C 70 05 BC 
[29, 21, 42] float: -0.002341 byte A8 72 19 BB 
[29, 22, 42] float: -0.003430 byte 06 D2 60 BB 
[29, 23, 42] float:  0.001774 byte A8 76 E8 3A 
[29, 24, 42] float:  0.005405 byte A8 1F B1 3B 
[29, 25, 42] float:  0.005186 byte 78 EB A9 3B 
[29, 26, 42] float:  0.009575 byte 6B E0 1C 3C 
[29, 27, 42] float:  0.002113 byte 54 76 0A 3B 
[29, 28, 42] float: -0.001930 byte DB 07 FD BA 
[29, 29, 42] float: -0.003988 byte 34 AB 82 BB 
[29, 30, 42] float: -0.003222 byte E0 21 53 BB 
[30,  1, 42] float: -0.011943 byte 85 AE 43 BC 
[30,  2, 42] float: -0.011243 byte A4 36 38 BC 
[30,  3, 42] float: -0.000441 byte E0 3E E7 B9 
[30,  4, 42] float:  0.005142 byte 44 7E A8 3B 
[30,  5, 42] float:  0.006851 byte 5C 7D E0 3B 
[30,  6, 42] float:  0.007807 byte B4 D5 FF 3B 
[30,  7, 42] float:  0.006048 byte BF 2B C6 3B 
[30,  8, 42] float:  0.002110 byte 2A 41 0A 3B 
[30,  9, 42] float: -0.000053 byte 00 DD 5D B8 
[30, 10, 42] float: -0.003967 byte FB FC 81 BB 
[30, 11, 42] float: -0.006074 byte FF 09 C7 BB 
[30, 12, 42] float: -0.007147 byte F9 33 EA BB 
[30, 13, 42] float: -0.009058 byte 9F 69 14 BC 
[30, 14, 42] float: -0.008055 byte 25 F7 03 BC 
[30, 15, 42] float: -0.009883 byte 3C ED 21 BC 
[30, 16, 42] float: -0.008156 byte 1A A2 05 BC 
[30, 17, 42] float: -0.009883 byte 3C ED 21 BC 
[30, 18, 42] float: -0.008055 byte 25 F7 03 BC 
[30, 19, 42] float: -0.009058 byte 9F 69 14 BC 
[30, 20, 42] float: -0.007147 byte F9 33 EA BB 
[30, 21, 42] float: -0.006074 byte FF 09 C7 BB 
[30, 22, 42] float: -0.003967 byte FB FC 81 BB 
[30, 23, 42] float: -0.000053 byte 00 DD 5D B8 
[30, 24, 42] float:  0.002110 byte 2A 41 0A 3B 
[30, 25, 42] float:  0.006048 byte BF 2B C6 3B 
[30, 26, 42] float:  0.007807 byte B4 D5 FF 3B 
[30, 27, 42] float:  0.006289 byte CE 14 CE 3B 
[30, 28, 42] float:  0.004272 byte 1A F8 8B 3B 
[30, 29, 42] float: -0.003222 byte E0 21 53 BB 
[30, 30, 42] float: -0.012886 byte 8C 21 53 BC
'''

data_str_2 = '''
[ 1,  1, 42] float:  0.001513 byte: CB 4D C6 3A 
[ 1,  2, 42] float: -0.008819 byte: 54 7D 10 BC 
[ 1,  3, 42] float: -0.011783 byte: 1C 0D 41 BC 
[ 1,  4, 42] float: -0.007413 byte: 56 E9 F2 BB 
[ 1,  5, 42] float: -0.001411 byte: 38 E5 B8 BA 
[ 1,  6, 42] float:  0.006092 byte: 44 A3 C7 3B 
[ 1,  7, 42] float:  0.010810 byte: 04 1B 31 3C 
[ 1,  8, 42] float:  0.014149 byte: 49 D2 67 3C 
[ 1,  9, 42] float:  0.013009 byte: 16 25 55 3C 
[ 1, 10, 42] float:  0.011870 byte: 40 7B 42 3C 
[ 1, 11, 42] float:  0.008223 byte: 06 B9 06 3C 
[ 1, 12, 42] float:  0.005581 byte: 4B E3 B6 3B 
[ 1, 13, 42] float:  0.002764 byte: 48 23 35 3B 
[ 1, 14, 42] float:  0.000682 byte: 90 CF 32 3A 
[ 1, 15, 42] float: -0.000317 byte: E0 52 A6 B9 
[ 1, 16, 42] float: -0.001109 byte: 50 58 91 BA 
[ 1, 17, 42] float: -0.000491 byte: B0 C7 00 BA 
[ 1, 18, 42] float:  0.000290 byte: 60 D8 97 39 
[ 1, 19, 42] float:  0.002118 byte: F4 D5 0A 3B 
[ 1, 20, 42] float:  0.004618 byte: C0 56 97 3B 
[ 1, 21, 42] float:  0.006998 byte: 4B 4D E5 3B 
[ 1, 22, 42] float:  0.010462 byte: F0 67 2B 3C 
[ 1, 23, 42] float:  0.011754 byte: BA 95 40 3C 
[ 1, 24, 42] float:  0.013257 byte: 68 33 59 3C 
[ 1, 25, 42] float:  0.010656 byte: E9 96 2E 3C 
[ 1, 26, 42] float:  0.006643 byte: 15 B0 D9 3B 
[ 1, 27, 42] float: -0.000860 byte: 70 59 61 BA 
[ 1, 28, 42] float: -0.007448 byte: BC 0A F4 BB 
[ 1, 29, 42] float: -0.012320 byte: 51 DB 49 BC 
[ 1, 30, 42] float: -0.011438 byte: A6 64 3B BC 
[ 2,  1, 42] float: -0.008819 byte: 54 7D 10 BC 
[ 2,  2, 42] float: -0.011246 byte: D2 41 38 BC 
[ 2,  3, 42] float: -0.011233 byte: 90 0B 38 BC 
[ 2,  4, 42] float: -0.003616 byte: 4C 02 6D BB 
[ 2,  5, 42] float:  0.000538 byte: 40 F8 0C 3A 
[ 2,  6, 42] float:  0.005868 byte: 30 4B C0 3B 
[ 2,  7, 42] float:  0.012128 byte: BE B3 46 3C 
[ 2,  8, 42] float:  0.010706 byte: 78 69 2F 3C 
[ 2,  9, 42] float:  0.012992 byte: 51 DC 54 3C 
[ 2, 10, 42] float:  0.008582 byte: 7C 9B 0C 3C 
[ 2, 11, 42] float:  0.007439 byte: 24 C6 F3 3B 
[ 2, 12, 42] float:  0.004086 byte: 68 E6 85 3B 
[ 2, 13, 42] float:  0.001992 byte: 7A 94 02 3B 
[ 2, 14, 42] float:  0.000830 byte: A8 9C 59 3A 
[ 2, 15, 42] float: -0.000801 byte: 78 04 52 BA 
[ 2, 16, 42] float: -0.000248 byte: 40 C4 81 B9 
[ 2, 17, 42] float: -0.000856 byte: F0 79 60 BA 
[ 2, 18, 42] float:  0.000687 byte: C8 FE 33 3A 
[ 2, 19, 42] float:  0.001567 byte: FC 6E CD 3A 
[ 2, 20, 42] float:  0.003313 byte: 14 1A 59 3B 
[ 2, 21, 42] float:  0.006148 byte: E0 78 C9 3B 
[ 2, 22, 42] float:  0.006806 byte: CC 05 DF 3B 
[ 2, 23, 42] float:  0.011008 byte: 33 5C 34 3C 
[ 2, 24, 42] float:  0.008840 byte: F3 D6 10 3C 
[ 2, 25, 42] float:  0.010955 byte: DA 7B 33 3C 
[ 2, 26, 42] float:  0.005889 byte: 8A FB C0 3B 
[ 2, 27, 42] float:  0.000762 byte: C0 D7 47 3A 
[ 2, 28, 42] float: -0.003862 byte: AC 15 7D BB 
[ 2, 29, 42] float: -0.013751 byte: 16 4C 61 BC 
[ 2, 30, 42] float: -0.013208 byte: 39 67 58 BC 
[ 3,  1, 42] float: -0.011783 byte: 1C 0D 41 BC 
[ 3,  2, 42] float: -0.011233 byte: 90 0B 38 BC 
[ 3,  3, 42] float: -0.000511 byte: 68 FC 05 BA 
[ 3,  4, 42] float: -0.000408 byte: 68 BB D5 B9 
[ 3,  5, 42] float:  0.001678 byte: 60 FD DB 3A 
[ 3,  6, 42] float:  0.006161 byte: E5 E1 C9 3B 
[ 3,  7, 42] float:  0.004021 byte: 71 C4 83 3B 
[ 3,  8, 42] float:  0.004487 byte: 0D 0B 93 3B 
[ 3,  9, 42] float:  0.002467 byte: 86 B2 21 3B 
[ 3, 10, 42] float:  0.000184 byte: D0 CE 40 39 
[ 3, 11, 42] float: -0.000045 byte: 40 F5 3B B8 
[ 3, 12, 42] float: -0.002139 byte: 0C 2D 0C BB 
[ 3, 13, 42] float: -0.001218 byte: 94 A7 9F BA 
[ 3, 14, 42] float: -0.002391 byte: F7 B7 1C BB 
[ 3, 15, 42] float: -0.001233 byte: 3E A2 A1 BA 
[ 3, 16, 42] float: -0.001994 byte: 03 AB 02 BB 
[ 3, 17, 42] float: -0.000852 byte: 04 75 5F BA 
[ 3, 18, 42] float: -0.001763 byte: 77 18 E7 BA 
[ 3, 19, 42] float: -0.000398 byte: 58 D1 D0 B9 
[ 3, 20, 42] float: -0.001583 byte: B0 84 CF BA 
[ 3, 21, 42] float:  0.000203 byte: D7 DF 54 39 
[ 3, 22, 42] float: -0.000487 byte: 4E 3D FF B9 
[ 3, 23, 42] float:  0.001021 byte: 6B D4 85 3A 
[ 3, 24, 42] float:  0.002416 byte: 8A 56 1E 3B 
[ 3, 25, 42] float:  0.001571 byte: E4 EE CD 3A 
[ 3, 26, 42] float:  0.004310 byte: 88 3D 8D 3B 
[ 3, 27, 42] float:  0.000887 byte: 42 A2 68 3A 
[ 3, 28, 42] float: -0.002863 byte: 99 A4 3B BB 
[ 3, 29, 42] float: -0.002500 byte: 38 D8 23 BB 
[ 3, 30, 42] float: -0.004257 byte: CE 7F 8B BB 
[ 4,  1, 42] float: -0.007413 byte: 56 E9 F2 BB 
[ 4,  2, 42] float: -0.003616 byte: 4C 02 6D BB 
[ 4,  3, 42] float: -0.000408 byte: 68 BB D5 B9 
[ 4,  4, 42] float:  0.001654 byte: 00 CF D8 3A 
[ 4,  5, 42] float:  0.006720 byte: 2D 32 DC 3B 
[ 4,  6, 42] float:  0.005855 byte: 88 DB BF 3B 
[ 4,  7, 42] float: -0.000336 byte: 30 F7 AF B9 
[ 4,  8, 42] float: -0.001198 byte: 40 11 9D BA 
[ 4,  9, 42] float: -0.008199 byte: 70 55 06 BC 
[ 4, 10, 42] float: -0.007567 byte: 81 F4 F7 BB 
[ 4, 11, 42] float: -0.009118 byte: 0E 64 15 BC 
[ 4, 12, 42] float: -0.008322 byte: D0 5A 08 BC 
[ 4, 13, 42] float: -0.005894 byte: 09 25 C1 BB 
[ 4, 14, 42] float: -0.006377 byte: 0F F9 D0 BB 
[ 4, 15, 42] float: -0.002954 byte: 85 9C 41 BB 
[ 4, 16, 42] float: -0.004931 byte: 7A 96 A1 BB 
[ 4, 17, 42] float: -0.002206 byte: 5F 95 10 BB 
[ 4, 18, 42] float: -0.004981 byte: 3E 3B A3 BB 
[ 4, 19, 42] float: -0.003753 byte: 59 F6 75 BB 
[ 4, 20, 42] float: -0.005859 byte: 0D FF BF BB 
[ 4, 21, 42] float: -0.006604 byte: FE 66 D8 BB 
[ 4, 22, 42] float: -0.005583 byte: D9 F3 B6 BB 
[ 4, 23, 42] float: -0.007720 byte: 9F FA FC BB 
[ 4, 24, 42] float: -0.001890 byte: 62 AE F7 BA 
[ 4, 25, 42] float: -0.002989 byte: 92 E4 43 BB 
[ 4, 26, 42] float:  0.002448 byte: E2 67 20 3B 
[ 4, 27, 42] float:  0.002929 byte: 9E F3 3F 3B 
[ 4, 28, 42] float: -0.001003 byte: 6C 6E 83 BA 
[ 4, 29, 42] float:  0.002955 byte: 2E AF 41 3B 
[ 4, 30, 42] float:  0.005250 byte: B4 0B AC 3B 
[ 5,  1, 42] float: -0.001411 byte: 38 E5 B8 BA 
[ 5,  2, 42] float:  0.000538 byte: 40 F8 0C 3A 
[ 5,  3, 42] float:  0.001678 byte: 60 FD DB 3A 
[ 5,  4, 42] float:  0.006720 byte: 2D 32 DC 3B 
[ 5,  5, 42] float:  0.008615 byte: EB 26 0D 3C 
[ 5,  6, 42] float: -0.000547 byte: B0 4A 0F BA 
[ 5,  7, 42] float: -0.004261 byte: 63 A3 8B BB 
[ 5,  8, 42] float: -0.011160 byte: 38 D8 36 BC 
[ 5,  9, 42] float: -0.013030 byte: B2 79 55 BC 
[ 5, 10, 42] float: -0.010910 byte: BC C0 32 BC 
[ 5, 11, 42] float: -0.010475 byte: D7 A0 2B BC 
[ 5, 12, 42] float: -0.004692 byte: 85 C2 99 BB 
[ 5, 13, 42] float: -0.004366 byte: E2 13 8F BB 
[ 5, 14, 42] float:  0.000402 byte: A0 85 D2 39 
[ 5, 15, 42] float: -0.000535 byte: 38 4A 0C BA 
[ 5, 16, 42] float:  0.002284 byte: E6 A9 15 3B 
[ 5, 17, 42] float: -0.000078 byte: C0 98 A4 B8 
[ 5, 18, 42] float:  0.001509 byte: F8 B8 C5 3A 
[ 5, 19, 42] float: -0.002406 byte: D8 AC 1D BB 
[ 5, 20, 42] float: -0.001441 byte: 38 E8 BC BA 
[ 5, 21, 42] float: -0.006527 byte: E2 DF D5 BB 
[ 5, 22, 42] float: -0.006033 byte: F5 B0 C5 BB 
[ 5, 23, 42] float: -0.009272 byte: 97 EB 17 BC 
[ 5, 24, 42] float: -0.009022 byte: D1 D0 13 BC 
[ 5, 25, 42] float: -0.004664 byte: 37 D8 98 BB 
[ 5, 26, 42] float: -0.005369 byte: CE EB AF BB 
[ 5, 27, 42] float:  0.003949 byte: F6 68 81 3B 
[ 5, 28, 42] float:  0.006189 byte: C4 C9 CA 3B 
[ 5, 29, 42] float:  0.005755 byte: B1 92 BC 3B 
[ 5, 30, 42] float:  0.009131 byte: D8 98 15 3C 
[ 6,  1, 42] float:  0.006092 byte: 44 A3 C7 3B 
[ 6,  2, 42] float:  0.005868 byte: 30 4B C0 3B 
[ 6,  3, 42] float:  0.006161 byte: E5 E1 C9 3B 
[ 6,  4, 42] float:  0.005855 byte: 88 DB BF 3B 
[ 6,  5, 42] float: -0.000547 byte: B0 4A 0F BA 
[ 6,  6, 42] float: -0.006371 byte: 0F C5 D0 BB 
[ 6,  7, 42] float: -0.010716 byte: 16 94 2F BC 
[ 6,  8, 42] float: -0.012151 byte: 7E 13 47 BC 
[ 6,  9, 42] float: -0.007933 byte: 74 F7 01 BC 
[ 6, 10, 42] float: -0.003787 byte: E2 35 78 BB 
[ 6, 11, 42] float:  0.000570 byte: 40 7A 15 3A 
[ 6, 12, 42] float:  0.005083 byte: 85 92 A6 3B 
[ 6, 13, 42] float:  0.005659 byte: 33 72 B9 3B 
[ 6, 14, 42] float:  0.008743 byte: 0B 3D 0F 3C 
[ 6, 15, 42] float:  0.006582 byte: F8 AF D7 3B 
[ 6, 16, 42] float:  0.008865 byte: 4B 3D 11 3C 
[ 6, 17, 42] float:  0.005862 byte: 30 18 C0 3B 
[ 6, 18, 42] float:  0.007862 byte: 3A CE 00 3C 
[ 6, 19, 42] float:  0.004760 byte: 60 FC 9B 3B 
[ 6, 20, 42] float:  0.005766 byte: 8B ED BC 3B 
[ 6, 21, 42] float:  0.002637 byte: 38 CF 2C 3B 
[ 6, 22, 42] float:  0.000798 byte: CC 44 51 3A 
[ 6, 23, 42] float: -0.001939 byte: 44 36 FE BA 
[ 6, 24, 42] float: -0.006652 byte: B0 F9 D9 BB 
[ 6, 25, 42] float: -0.007266 byte: 5E 17 EE BB 
[ 6, 26, 42] float: -0.008325 byte: 66 64 08 BC 
[ 6, 27, 42] float: -0.005379 byte: 7C 45 B0 BB 
[ 6, 28, 42] float:  0.005556 byte: 27 11 B6 3B 
[ 6, 29, 42] float:  0.013622 byte: B1 2E 5F 3C 
[ 6, 30, 42] float:  0.010741 byte: 70 F9 2F 3C 
[ 7,  1, 42] float:  0.010810 byte: 04 1B 31 3C 
[ 7,  2, 42] float:  0.012128 byte: BE B3 46 3C 
[ 7,  3, 42] float:  0.004021 byte: 71 C4 83 3B 
[ 7,  4, 42] float: -0.000336 byte: 30 F7 AF B9 
[ 7,  5, 42] float: -0.004261 byte: 63 A3 8B BB 
[ 7,  6, 42] float: -0.010716 byte: 16 94 2F BC 
[ 7,  7, 42] float: -0.009966 byte: B9 49 23 BC 
[ 7,  8, 42] float: -0.005740 byte: 02 18 BC BB 
[ 7,  9, 42] float:  0.000667 byte: 98 EF 2E 3A 
[ 7, 10, 42] float:  0.004713 byte: 71 6D 9A 3B 
[ 7, 11, 42] float:  0.008304 byte: 92 0E 08 3C 
[ 7, 12, 42] float:  0.008805 byte: 77 42 10 3C 
[ 7, 13, 42] float:  0.008615 byte: BB 25 0D 3C 
[ 7, 14, 42] float:  0.007293 byte: 7A FD EE 3B 
[ 7, 15, 42] float:  0.005776 byte: 04 47 BD 3B 
[ 7, 16, 42] float:  0.005052 byte: 74 88 A5 3B 
[ 7, 17, 42] float:  0.004044 byte: CA 80 84 3B 
[ 7, 18, 42] float:  0.004315 byte: E6 68 8D 3B 
[ 7, 19, 42] float:  0.004410 byte: 96 82 90 3B 
[ 7, 20, 42] float:  0.004832 byte: 0C 53 9E 3B 
[ 7, 21, 42] float:  0.005655 byte: 1A 4B B9 3B 
[ 7, 22, 42] float:  0.004255 byte: AC 70 8B 3B 
[ 7, 23, 42] float:  0.004149 byte: 74 F1 87 3B 
[ 7, 24, 42] float: -0.001087 byte: 18 88 8E BA 
[ 7, 25, 42] float: -0.002979 byte: 22 41 43 BB 
[ 7, 26, 42] float: -0.007078 byte: B3 ED E7 BB 
[ 7, 27, 42] float: -0.006507 byte: 6F 35 D5 BB 
[ 7, 28, 42] float:  0.002145 byte: 62 98 0C 3B 
[ 7, 29, 42] float:  0.010163 byte: BE 80 26 3C 
[ 7, 30, 42] float:  0.011326 byte: EC 8F 39 3C 
[ 8,  1, 42] float:  0.014149 byte: 49 D2 67 3C 
[ 8,  2, 42] float:  0.010706 byte: 78 69 2F 3C 
[ 8,  3, 42] float:  0.004487 byte: 0D 0B 93 3B 
[ 8,  4, 42] float: -0.001198 byte: 40 11 9D BA 
[ 8,  5, 42] float: -0.011160 byte: 38 D8 36 BC 
[ 8,  6, 42] float: -0.012151 byte: 7E 13 47 BC 
[ 8,  7, 42] float: -0.005740 byte: 02 18 BC BB 
[ 8,  8, 42] float:  0.000555 byte: B8 90 11 3A 
[ 8,  9, 42] float:  0.006960 byte: 93 13 E4 3B 
[ 8, 10, 42] float:  0.008317 byte: DD 43 08 3C 
[ 8, 11, 42] float:  0.010131 byte: 76 FE 25 3C 
[ 8, 12, 42] float:  0.007642 byte: E2 69 FA 3B 
[ 8, 13, 42] float:  0.005805 byte: 67 34 BE 3B 
[ 8, 14, 42] float:  0.003072 byte: 3E 5A 49 3B 
[ 8, 15, 42] float:  0.001397 byte: 56 22 B7 3A 
[ 8, 16, 42] float:  0.000218 byte: A0 0C 65 39 
[ 8, 17, 42] float: -0.000248 byte: 10 15 82 B9 
[ 8, 18, 42] float: -0.000363 byte: 90 34 BE B9 
[ 8, 19, 42] float:  0.000908 byte: 56 EE 6D 3A 
[ 8, 20, 42] float:  0.001096 byte: 8C AF 8F 3A 
[ 8, 21, 42] float:  0.004211 byte: F4 FA 89 3B 
[ 8, 22, 42] float:  0.003082 byte: 94 FF 49 3B 
[ 8, 23, 42] float:  0.005410 byte: 2C 44 B1 3B 
[ 8, 24, 42] float:  0.001075 byte: 77 D7 8C 3A 
[ 8, 25, 42] float: -0.002546 byte: 89 D9 26 BB 
[ 8, 26, 42] float: -0.004499 byte: EA 69 93 BB 
[ 8, 27, 42] float: -0.005308 byte: E9 EA AD BB 
[ 8, 28, 42] float:  0.001800 byte: 1D FA EB 3A 
[ 8, 29, 42] float:  0.008216 byte: 9C 9A 06 3C 
[ 8, 30, 42] float:  0.006572 byte: CC 56 D7 3B 
[ 9,  1, 42] float:  0.013009 byte: 16 25 55 3C 
[ 9,  2, 42] float:  0.012992 byte: 51 DC 54 3C 
[ 9,  3, 42] float:  0.002467 byte: 86 B2 21 3B 
[ 9,  4, 42] float: -0.008199 byte: 70 55 06 BC 
[ 9,  5, 42] float: -0.013030 byte: B2 79 55 BC 
[ 9,  6, 42] float: -0.007933 byte: 74 F7 01 BC 
[ 9,  7, 42] float:  0.000667 byte: 98 EF 2E 3A 
[ 9,  8, 42] float:  0.006960 byte: 93 13 E4 3B 
[ 9,  9, 42] float:  0.009506 byte: B3 BD 1B 3C 
[ 9, 10, 42] float:  0.009710 byte: C8 14 1F 3C 
[ 9, 11, 42] float:  0.008535 byte: 72 D6 0B 3C 
[ 9, 12, 42] float:  0.003116 byte: 1E 31 4C 3B 
[ 9, 13, 42] float:  0.001633 byte: 24 0C D6 3A 
[ 9, 14, 42] float: -0.002826 byte: 18 35 39 BB 
[ 9, 15, 42] float: -0.002174 byte: DB 81 0E BB 
[ 9, 16, 42] float: -0.005218 byte: C0 F7 AA BB 
[ 9, 17, 42] float: -0.003214 byte: 23 A1 52 BB 
[ 9, 18, 42] float: -0.005554 byte: 2A FA B5 BB 
[ 9, 19, 42] float: -0.002542 byte: AE 93 26 BB 
[ 9, 20, 42] float: -0.003603 byte: DE 27 6C BB 
[ 9, 21, 42] float:  0.001068 byte: 6A EC 8B 3A 
[ 9, 22, 42] float:  0.001784 byte: FA D0 E9 3A 
[ 9, 23, 42] float:  0.004818 byte: 3B DF 9D 3B 
[ 9, 24, 42] float:  0.004483 byte: FA E3 92 3B 
[ 9, 25, 42] float: -0.000207 byte: 70 90 59 B9 
[ 9, 26, 42] float: -0.001960 byte: 40 73 00 BB 
[ 9, 27, 42] float: -0.004260 byte: 2A 94 8B BB 
[ 9, 28, 42] float: -0.000160 byte: 20 70 27 B9 
[ 9, 29, 42] float:  0.004661 byte: 0C BE 98 3B 
[ 9, 30, 42] float:  0.003172 byte: F4 E2 4F 3B 
[10,  1, 42] float:  0.011870 byte: 40 7B 42 3C 
[10,  2, 42] float:  0.008582 byte: 7C 9B 0C 3C 
[10,  3, 42] float:  0.000184 byte: D0 CE 40 39 
[10,  4, 42] float: -0.007567 byte: 81 F4 F7 BB 
[10,  5, 42] float: -0.010910 byte: BC C0 32 BC 
[10,  6, 42] float: -0.003787 byte: E2 35 78 BB 
[10,  7, 42] float:  0.004713 byte: 71 6D 9A 3B 
[10,  8, 42] float:  0.008317 byte: DD 43 08 3C 
[10,  9, 42] float:  0.009710 byte: C8 14 1F 3C 
[10, 10, 42] float:  0.007692 byte: 54 09 FC 3B 
[10, 11, 42] float:  0.002441 byte: C0 F2 1F 3B 
[10, 12, 42] float: -0.001640 byte: 30 EA D6 BA 
[10, 13, 42] float: -0.003981 byte: 4F 75 82 BB 
[10, 14, 42] float: -0.005658 byte: 2A 69 B9 BB 
[10, 15, 42] float: -0.005577 byte: 57 BB B6 BB 
[10, 16, 42] float: -0.006194 byte: 8F F5 CA BB 
[10, 17, 42] float: -0.005405 byte: 64 19 B1 BB 
[10, 18, 42] float: -0.006383 byte: D0 29 D1 BB 
[10, 19, 42] float: -0.005869 byte: 40 4F C0 BB 
[10, 20, 42] float: -0.006556 byte: B6 D4 D6 BB 
[10, 21, 42] float: -0.004543 byte: 2C DE 94 BB 
[10, 22, 42] float: -0.001869 byte: 4C F4 F4 BA 
[10, 23, 42] float:  0.000874 byte: A8 19 65 3A 
[10, 24, 42] float:  0.003620 byte: 5A 36 6D 3B 
[10, 25, 42] float:  0.001236 byte: 14 04 A2 3A 
[10, 26, 42] float: -0.000228 byte: 30 8C 6F B9 
[10, 27, 42] float:  0.000739 byte: 90 9C 41 3A 
[10, 28, 42] float:  0.000674 byte: 4E A1 30 3A 
[10, 29, 42] float: -0.000619 byte: C8 52 22 BA 
[10, 30, 42] float: -0.001260 byte: BC 1B A5 BA 
[11,  1, 42] float:  0.008223 byte: 06 B9 06 3C 
[11,  2, 42] float:  0.007439 byte: 24 C6 F3 3B 
[11,  3, 42] float: -0.000045 byte: 40 F5 3B B8 
[11,  4, 42] float: -0.009118 byte: 0E 64 15 BC 
[11,  5, 42] float: -0.010475 byte: D7 A0 2B BC 
[11,  6, 42] float:  0.000570 byte: 40 7A 15 3A 
[11,  7, 42] float:  0.008304 byte: 92 0E 08 3C 
[11,  8, 42] float:  0.010131 byte: 76 FE 25 3C 
[11,  9, 42] float:  0.008535 byte: 72 D6 0B 3C 
[11, 10, 42] float:  0.002441 byte: C0 F2 1F 3B 
[11, 11, 42] float: -0.001915 byte: 20 F9 FA BA 
[11, 12, 42] float: -0.005242 byte: 81 C6 AB BB 
[11, 13, 42] float: -0.005530 byte: 3E 38 B5 BB 
[11, 14, 42] float: -0.006656 byte: 02 1A DA BB 
[11, 15, 42] float: -0.004768 byte: 11 3C 9C BB 
[11, 16, 42] float: -0.004992 byte: 8B 95 A3 BB 
[11, 17, 42] float: -0.003116 byte: C0 35 4C BB 
[11, 18, 42] float: -0.004474 byte: FF 9D 92 BB 
[11, 19, 42] float: -0.003760 byte: 11 67 76 BB 
[11, 20, 42] float: -0.006056 byte: C0 74 C6 BB 
[11, 21, 42] float: -0.005414 byte: 3C 66 B1 BB 
[11, 22, 42] float: -0.004282 byte: 03 4C 8C BB 
[11, 23, 42] float: -0.000828 byte: 90 F5 58 BA 
[11, 24, 42] float:  0.002530 byte: D8 C7 25 3B 
[11, 25, 42] float:  0.001712 byte: 51 5B E0 3A 
[11, 26, 42] float:  0.001917 byte: FC 3D FB 3A 
[11, 27, 42] float: -0.000081 byte: 00 75 A9 B8 
[11, 28, 42] float:  0.000411 byte: F8 7F D7 39 
[11, 29, 42] float: -0.001302 byte: 9C B6 AA BA 
[11, 30, 42] float: -0.005074 byte: 34 44 A6 BB 
[12,  1, 42] float:  0.005581 byte: 4B E3 B6 3B 
[12,  2, 42] float:  0.004086 byte: 68 E6 85 3B 
[12,  3, 42] float: -0.002139 byte: 0C 2D 0C BB 
[12,  4, 42] float: -0.008322 byte: D0 5A 08 BC 
[12,  5, 42] float: -0.004692 byte: 85 C2 99 BB 
[12,  6, 42] float:  0.005083 byte: 85 92 A6 3B 
[12,  7, 42] float:  0.008805 byte: 77 42 10 3C 
[12,  8, 42] float:  0.007642 byte: E2 69 FA 3B 
[12,  9, 42] float:  0.003116 byte: 1E 31 4C 3B 
[12, 10, 42] float: -0.001640 byte: 30 EA D6 BA 
[12, 11, 42] float: -0.005242 byte: 81 C6 AB BB 
[12, 12, 42] float: -0.006213 byte: CC 92 CB BB 
[12, 13, 42] float: -0.006697 byte: 4E 76 DB BB 
[12, 14, 42] float: -0.005369 byte: 51 F2 AF BB 
[12, 15, 42] float: -0.003658 byte: C5 C1 6F BB 
[12, 16, 42] float: -0.002148 byte: AB CA 0C BB 
[12, 17, 42] float: -0.001098 byte: 1C E6 8F BA 
[12, 18, 42] float: -0.001305 byte: A8 00 AB BA 
[12, 19, 42] float: -0.002346 byte: 98 BD 19 BB 
[12, 20, 42] float: -0.003735 byte: 4E C3 74 BB 
[12, 21, 42] float: -0.005987 byte: C9 2A C4 BB 
[12, 22, 42] float: -0.005237 byte: 7D 9D AB BB 
[12, 23, 42] float: -0.004069 byte: D0 54 85 BB 
[12, 24, 42] float:  0.001558 byte: 1B 27 CC 3A 
[12, 25, 42] float:  0.002296 byte: F9 76 16 3B 
[12, 26, 42] float:  0.002678 byte: 0C 85 2F 3B 
[12, 27, 42] float:  0.004770 byte: 21 4A 9C 3B 
[12, 28, 42] float:  0.000242 byte: B0 2B 7E 39 
[12, 29, 42] float: -0.006336 byte: 16 A0 CF BB 
[12, 30, 42] float: -0.006739 byte: 1D D5 DC BB 
[13,  1, 42] float:  0.002764 byte: 48 23 35 3B 
[13,  2, 42] float:  0.001992 byte: 7A 94 02 3B 
[13,  3, 42] float: -0.001218 byte: 94 A7 9F BA 
[13,  4, 42] float: -0.005894 byte: 09 25 C1 BB 
[13,  5, 42] float: -0.004366 byte: E2 13 8F BB 
[13,  6, 42] float:  0.005659 byte: 33 72 B9 3B 
[13,  7, 42] float:  0.008615 byte: BB 25 0D 3C 
[13,  8, 42] float:  0.005805 byte: 67 34 BE 3B 
[13,  9, 42] float:  0.001633 byte: 24 0C D6 3A 
[13, 10, 42] float: -0.003981 byte: 4F 75 82 BB 
[13, 11, 42] float: -0.005530 byte: 3E 38 B5 BB 
[13, 12, 42] float: -0.006697 byte: 4E 76 DB BB 
[13, 13, 42] float: -0.004778 byte: 77 8D 9C BB 
[13, 14, 42] float: -0.003270 byte: FE 46 56 BB 
[13, 15, 42] float: -0.000439 byte: 64 FA E5 B9 
[13, 16, 42] float:  0.000746 byte: BC 89 43 3A 
[13, 17, 42] float:  0.002645 byte: 7A 52 2D 3B 
[13, 18, 42] float:  0.001645 byte: 36 A1 D7 3A 
[13, 19, 42] float:  0.001110 byte: DE 7D 91 3A 
[13, 20, 42] float: -0.001505 byte: B6 4C C5 BA 
[13, 21, 42] float: -0.003193 byte: 12 3D 51 BB 
[13, 22, 42] float: -0.005017 byte: 58 66 A4 BB 
[13, 23, 42] float: -0.003719 byte: F3 B5 73 BB 
[13, 24, 42] float:  0.000403 byte: 3C 68 D3 39 
[13, 25, 42] float:  0.002569 byte: CE 5C 28 3B 
[13, 26, 42] float:  0.003977 byte: D6 4E 82 3B 
[13, 27, 42] float:  0.002435 byte: 97 8C 1F 3B 
[13, 28, 42] float:  0.000680 byte: 0B 3E 32 3A 
[13, 29, 42] float: -0.005209 byte: 25 B1 AA BB 
[13, 30, 42] float: -0.009683 byte: 68 A5 1E BC 
[14,  1, 42] float:  0.000682 byte: 90 CF 32 3A 
[14,  2, 42] float:  0.000830 byte: A8 9C 59 3A 
[14,  3, 42] float: -0.002391 byte: F7 B7 1C BB 
[14,  4, 42] float: -0.006377 byte: 0F F9 D0 BB 
[14,  5, 42] float:  0.000402 byte: A0 85 D2 39 
[14,  6, 42] float:  0.008743 byte: 0B 3D 0F 3C 
[14,  7, 42] float:  0.007293 byte: 7A FD EE 3B 
[14,  8, 42] float:  0.003072 byte: 3E 5A 49 3B 
[14,  9, 42] float: -0.002826 byte: 18 35 39 BB 
[14, 10, 42] float: -0.005658 byte: 2A 69 B9 BB 
[14, 11, 42] float: -0.006656 byte: 02 1A DA BB 
[14, 12, 42] float: -0.005369 byte: 51 F2 AF BB 
[14, 13, 42] float: -0.003270 byte: FE 46 56 BB 
[14, 14, 42] float: -0.001016 byte: 2E 23 85 BA 
[14, 15, 42] float:  0.001554 byte: C9 AD CB 3A 
[14, 16, 42] float:  0.003288 byte: 0E 77 57 3B 
[14, 17, 42] float:  0.003980 byte: B2 68 82 3B 
[14, 18, 42] float:  0.002763 byte: D1 17 35 3B 
[14, 19, 42] float:  0.001230 byte: 82 46 A1 3A 
[14, 20, 42] float: -0.000391 byte: D0 CD CC B9 
[14, 21, 42] float: -0.003108 byte: 63 AB 4B BB 
[14, 22, 42] float: -0.003749 byte: BF B6 75 BB 
[14, 23, 42] float: -0.005222 byte: D8 19 AB BB 
[14, 24, 42] float:  0.000520 byte: DC 39 08 3A 
[14, 25, 42] float:  0.003500 byte: C6 65 65 3B 
[14, 26, 42] float:  0.004972 byte: D0 EB A2 3B 
[14, 27, 42] float:  0.006428 byte: AE 9E D2 3B 
[14, 28, 42] float: -0.000572 byte: 27 0D 16 BA 
[14, 29, 42] float: -0.009274 byte: 6E F3 17 BC 
[14, 30, 42] float: -0.009383 byte: AC BB 19 BC 
[15,  1, 42] float: -0.000317 byte: E0 52 A6 B9 
[15,  2, 42] float: -0.000801 byte: 78 04 52 BA 
[15,  3, 42] float: -0.001233 byte: 3E A2 A1 BA 
[15,  4, 42] float: -0.002954 byte: 85 9C 41 BB 
[15,  5, 42] float: -0.000535 byte: 38 4A 0C BA 
[15,  6, 42] float:  0.006582 byte: F8 AF D7 3B 
[15,  7, 42] float:  0.005776 byte: 04 47 BD 3B 
[15,  8, 42] float:  0.001397 byte: 56 22 B7 3A 
[15,  9, 42] float: -0.002174 byte: DB 81 0E BB 
[15, 10, 42] float: -0.005577 byte: 57 BB B6 BB 
[15, 11, 42] float: -0.004768 byte: 11 3C 9C BB 
[15, 12, 42] float: -0.003658 byte: C5 C1 6F BB 
[15, 13, 42] float: -0.000439 byte: 64 FA E5 B9 
[15, 14, 42] float:  0.001554 byte: C9 AD CB 3A 
[15, 15, 42] float:  0.006437 byte: 1C EB D2 3B 
[15, 16, 42] float:  0.008475 byte: 1F DC 0A 3C 
[15, 17, 42] float:  0.008913 byte: 50 09 12 3C 
[15, 18, 42] float:  0.003604 byte: 5F 2D 6C 3B 
[15, 19, 42] float:  0.001846 byte: D8 03 F2 3A 
[15, 20, 42] float: -0.001006 byte: 12 DE 83 BA 
[15, 21, 42] float: -0.002346 byte: FA C1 19 BB 
[15, 22, 42] float: -0.003712 byte: B7 3D 73 BB 
[15, 23, 42] float: -0.003116 byte: 0F 36 4C BB 
[15, 24, 42] float:  0.000447 byte: 00 22 EA 39 
[15, 25, 42] float:  0.003735 byte: CC C6 74 3B 
[15, 26, 42] float:  0.005709 byte: 5C 0F BB 3B 
[15, 27, 42] float:  0.003733 byte: E0 A4 74 3B 
[15, 28, 42] float:  0.000232 byte: 12 BC 73 39 
[15, 29, 42] float: -0.007351 byte: A8 DE F0 BB 
[15, 30, 42] float: -0.011537 byte: 8F 04 3D BC 
[16,  1, 42] float: -0.001109 byte: 50 58 91 BA 
[16,  2, 42] float: -0.000248 byte: 40 C4 81 B9 
[16,  3, 42] float: -0.001994 byte: 03 AB 02 BB 
[16,  4, 42] float: -0.004931 byte: 7A 96 A1 BB 
[16,  5, 42] float:  0.002284 byte: E6 A9 15 3B 
[16,  6, 42] float:  0.008865 byte: 4B 3D 11 3C 
[16,  7, 42] float:  0.005052 byte: 74 88 A5 3B 
[16,  8, 42] float:  0.000218 byte: A0 0C 65 39 
[16,  9, 42] float: -0.005218 byte: C0 F7 AA BB 
[16, 10, 42] float: -0.006194 byte: 8F F5 CA BB 
[16, 11, 42] float: -0.004992 byte: 8B 95 A3 BB 
[16, 12, 42] float: -0.002148 byte: AB CA 0C BB 
[16, 13, 42] float:  0.000746 byte: BC 89 43 3A 
[16, 14, 42] float:  0.003288 byte: 0E 77 57 3B 
[16, 15, 42] float:  0.008475 byte: 1F DC 0A 3C 
[16, 16, 42] float:  0.010259 byte: EC 15 28 3C 
[16, 17, 42] float:  0.008753 byte: B2 68 0F 3C 
[16, 18, 42] float:  0.004280 byte: B0 3F 8C 3B 
[16, 19, 42] float:  0.000327 byte: E8 80 AB 39 
[16, 20, 42] float: -0.002088 byte: 3B D4 08 BB 
[16, 21, 42] float: -0.004207 byte: C6 DC 89 BB 
[16, 22, 42] float: -0.003781 byte: 12 C9 77 BB 
[16, 23, 42] float: -0.004620 byte: 9B 63 97 BB 
[16, 24, 42] float:  0.001559 byte: E0 67 CC 3A 
[16, 25, 42] float:  0.005194 byte: 32 33 AA 3B 
[16, 26, 42] float:  0.006703 byte: FE A0 DB 3B 
[16, 27, 42] float:  0.006960 byte: DF 0E E4 3B 
[16, 28, 42] float: -0.001451 byte: 3E 34 BE BA 
[16, 29, 42] float: -0.010564 byte: EA 12 2D BC 
[16, 30, 42] float: -0.010210 byte: 44 49 27 BC 
[17,  1, 42] float: -0.000491 byte: B0 C7 00 BA 
[17,  2, 42] float: -0.000856 byte: F0 79 60 BA 
[17,  3, 42] float: -0.000852 byte: 04 75 5F BA 
[17,  4, 42] float: -0.002206 byte: 5F 95 10 BB 
[17,  5, 42] float: -0.000078 byte: C0 98 A4 B8 
[17,  6, 42] float:  0.005862 byte: 30 18 C0 3B 
[17,  7, 42] float:  0.004044 byte: CA 80 84 3B 
[17,  8, 42] float: -0.000248 byte: 10 15 82 B9 
[17,  9, 42] float: -0.003214 byte: 23 A1 52 BB 
[17, 10, 42] float: -0.005405 byte: 64 19 B1 BB 
[17, 11, 42] float: -0.003116 byte: C0 35 4C BB 
[17, 12, 42] float: -0.001098 byte: 1C E6 8F BA 
[17, 13, 42] float:  0.002645 byte: 7A 52 2D 3B 
[17, 14, 42] float:  0.003980 byte: B2 68 82 3B 
[17, 15, 42] float:  0.008913 byte: 50 09 12 3C 
[17, 16, 42] float:  0.008753 byte: B2 68 0F 3C 
[17, 17, 42] float:  0.005585 byte: 2B 04 B7 3B 
[17, 18, 42] float: -0.000766 byte: 90 CB 48 BA 
[17, 19, 42] float: -0.002678 byte: 1B 83 2F BB 
[17, 20, 42] float: -0.005109 byte: 79 6C A7 BB 
[17, 21, 42] float: -0.004782 byte: 95 B1 9C BB 
[17, 22, 42] float: -0.004313 byte: F2 54 8D BB 
[17, 23, 42] float: -0.001964 byte: 6A B3 00 BB 
[17, 24, 42] float:  0.002274 byte: 39 FF 14 3B 
[17, 25, 42] float:  0.005374 byte: 54 15 B0 3B 
[17, 26, 42] float:  0.006812 byte: 70 33 DF 3B 
[17, 27, 42] float:  0.003583 byte: 62 CD 6A 3B 
[17, 28, 42] float: -0.000689 byte: D7 7C 34 BA 
[17, 29, 42] float: -0.007888 byte: FC 3D 01 BC 
[17, 30, 42] float: -0.011573 byte: A4 9C 3D BC 
[18,  1, 42] float:  0.000290 byte: 60 D8 97 39 
[18,  2, 42] float:  0.000687 byte: C8 FE 33 3A 
[18,  3, 42] float: -0.001763 byte: 77 18 E7 BA 
[18,  4, 42] float: -0.004981 byte: 3E 3B A3 BB 
[18,  5, 42] float:  0.001509 byte: F8 B8 C5 3A 
[18,  6, 42] float:  0.007862 byte: 3A CE 00 3C 
[18,  7, 42] float:  0.004315 byte: E6 68 8D 3B 
[18,  8, 42] float: -0.000363 byte: 90 34 BE B9 
[18,  9, 42] float: -0.005554 byte: 2A FA B5 BB 
[18, 10, 42] float: -0.006383 byte: D0 29 D1 BB 
[18, 11, 42] float: -0.004474 byte: FF 9D 92 BB 
[18, 12, 42] float: -0.001305 byte: A8 00 AB BA 
[18, 13, 42] float:  0.001645 byte: 36 A1 D7 3A 
[18, 14, 42] float:  0.002763 byte: D1 17 35 3B 
[18, 15, 42] float:  0.003604 byte: 5F 2D 6C 3B 
[18, 16, 42] float:  0.004280 byte: B0 3F 8C 3B 
[18, 17, 42] float: -0.000766 byte: 90 CB 48 BA 
[18, 18, 42] float: -0.004640 byte: C0 09 98 BB 
[18, 19, 42] float: -0.006372 byte: 32 CF D0 BB 
[18, 20, 42] float: -0.006973 byte: A3 7D E4 BB 
[18, 21, 42] float: -0.006741 byte: C6 E2 DC BB 
[18, 22, 42] float: -0.004309 byte: DC 2F 8D BB 
[18, 23, 42] float: -0.002842 byte: 34 3A 3A BB 
[18, 24, 42] float:  0.003934 byte: D7 E7 80 3B 
[18, 25, 42] float:  0.006693 byte: AA 4F DB 3B 
[18, 26, 42] float:  0.007021 byte: BA 12 E6 3B 
[18, 27, 42] float:  0.005972 byte: 36 B3 C3 3B 
[18, 28, 42] float: -0.002403 byte: 72 83 1D BB 
[18, 29, 42] float: -0.010288 byte: 66 90 28 BC 
[18, 30, 42] float: -0.009387 byte: 3D CA 19 BC 
[19,  1, 42] float:  0.002118 byte: F4 D5 0A 3B 
[19,  2, 42] float:  0.001567 byte: FC 6E CD 3A 
[19,  3, 42] float: -0.000398 byte: 58 D1 D0 B9 
[19,  4, 42] float: -0.003753 byte: 59 F6 75 BB 
[19,  5, 42] float: -0.002406 byte: D8 AC 1D BB 
[19,  6, 42] float:  0.004760 byte: 60 FC 9B 3B 
[19,  7, 42] float:  0.004410 byte: 96 82 90 3B 
[19,  8, 42] float:  0.000908 byte: 56 EE 6D 3A 
[19,  9, 42] float: -0.002542 byte: AE 93 26 BB 
[19, 10, 42] float: -0.005869 byte: 40 4F C0 BB 
[19, 11, 42] float: -0.003760 byte: 11 67 76 BB 
[19, 12, 42] float: -0.002346 byte: 98 BD 19 BB 
[19, 13, 42] float:  0.001110 byte: DE 7D 91 3A 
[19, 14, 42] float:  0.001230 byte: 82 46 A1 3A 
[19, 15, 42] float:  0.001846 byte: D8 03 F2 3A 
[19, 16, 42] float:  0.000327 byte: E8 80 AB 39 
[19, 17, 42] float: -0.002678 byte: 1B 83 2F BB 
[19, 18, 42] float: -0.006372 byte: 32 CF D0 BB 
[19, 19, 42] float: -0.007038 byte: 24 A0 E6 BB 
[19, 20, 42] float: -0.008117 byte: 3D FC 04 BC 
[19, 21, 42] float: -0.005616 byte: B0 09 B8 BB 
[19, 22, 42] float: -0.003548 byte: 37 83 68 BB 
[19, 23, 42] float:  0.001170 byte: 04 4F 99 3A 
[19, 24, 42] float:  0.005795 byte: 06 E5 BD 3B 
[19, 25, 42] float:  0.006785 byte: D6 52 DE 3B 
[19, 26, 42] float:  0.006053 byte: 2C 58 C6 3B 
[19, 27, 42] float:  0.000851 byte: FC FB 5E 3A 
[19, 28, 42] float: -0.002167 byte: CC 06 0E BB 
[19, 29, 42] float: -0.006213 byte: 81 96 CB BB 
[19, 30, 42] float: -0.009347 byte: E8 23 19 BC 
[20,  1, 42] float:  0.004618 byte: C0 56 97 3B 
[20,  2, 42] float:  0.003313 byte: 14 1A 59 3B 
[20,  3, 42] float: -0.001583 byte: B0 84 CF BA 
[20,  4, 42] float: -0.005859 byte: 0D FF BF BB 
[20,  5, 42] float: -0.001441 byte: 38 E8 BC BA 
[20,  6, 42] float:  0.005766 byte: 8B ED BC 3B 
[20,  7, 42] float:  0.004832 byte: 0C 53 9E 3B 
[20,  8, 42] float:  0.001096 byte: 8C AF 8F 3A 
[20,  9, 42] float: -0.003603 byte: DE 27 6C BB 
[20, 10, 42] float: -0.006556 byte: B6 D4 D6 BB 
[20, 11, 42] float: -0.006056 byte: C0 74 C6 BB 
[20, 12, 42] float: -0.003735 byte: 4E C3 74 BB 
[20, 13, 42] float: -0.001505 byte: B6 4C C5 BA 
[20, 14, 42] float: -0.000391 byte: D0 CD CC B9 
[20, 15, 42] float: -0.001006 byte: 12 DE 83 BA 
[20, 16, 42] float: -0.002088 byte: 3B D4 08 BB 
[20, 17, 42] float: -0.005109 byte: 79 6C A7 BB 
[20, 18, 42] float: -0.006973 byte: A3 7D E4 BB 
[20, 19, 42] float: -0.008117 byte: 3D FC 04 BC 
[20, 20, 42] float: -0.007192 byte: 98 AB EB BB 
[20, 21, 42] float: -0.005230 byte: FD 62 AB BB 
[20, 22, 42] float: -0.001286 byte: 38 85 A8 BA 
[20, 23, 42] float:  0.002335 byte: 80 FE 18 3B 
[20, 24, 42] float:  0.007279 byte: 54 82 EE 3B 
[20, 25, 42] float:  0.006719 byte: A4 2B DC 3B 
[20, 26, 42] float:  0.004157 byte: 5A 34 88 3B 
[20, 27, 42] float:  0.001975 byte: BC 6E 01 3B 
[20, 28, 42] float: -0.003080 byte: 34 DB 49 BB 
[20, 29, 42] float: -0.007079 byte: 6A F4 E7 BB 
[20, 30, 42] float: -0.006017 byte: C6 28 C5 BB 
[21,  1, 42] float:  0.006998 byte: 4B 4D E5 3B 
[21,  2, 42] float:  0.006148 byte: E0 78 C9 3B 
[21,  3, 42] float:  0.000203 byte: D7 DF 54 39 
[21,  4, 42] float: -0.006604 byte: FE 66 D8 BB 
[21,  5, 42] float: -0.006527 byte: E2 DF D5 BB 
[21,  6, 42] float:  0.002637 byte: 38 CF 2C 3B 
[21,  7, 42] float:  0.005655 byte: 1A 4B B9 3B 
[21,  8, 42] float:  0.004211 byte: F4 FA 89 3B 
[21,  9, 42] float:  0.001068 byte: 6A EC 8B 3A 
[21, 10, 42] float: -0.004543 byte: 2C DE 94 BB 
[21, 11, 42] float: -0.005414 byte: 3C 66 B1 BB 
[21, 12, 42] float: -0.005987 byte: C9 2A C4 BB 
[21, 13, 42] float: -0.003193 byte: 12 3D 51 BB 
[21, 14, 42] float: -0.003108 byte: 63 AB 4B BB 
[21, 15, 42] float: -0.002346 byte: FA C1 19 BB 
[21, 16, 42] float: -0.004207 byte: C6 DC 89 BB 
[21, 17, 42] float: -0.004782 byte: 95 B1 9C BB 
[21, 18, 42] float: -0.006741 byte: C6 E2 DC BB 
[21, 19, 42] float: -0.005616 byte: B0 09 B8 BB 
[21, 20, 42] float: -0.005230 byte: FD 62 AB BB 
[21, 21, 42] float: -0.001093 byte: 74 49 8F BA 
[21, 22, 42] float:  0.002130 byte: F0 98 0B 3B 
[21, 23, 42] float:  0.007094 byte: 8A 78 E8 3B 
[21, 24, 42] float:  0.009060 byte: 6C 72 14 3C 
[21, 25, 42] float:  0.005826 byte: 9E E5 BE 3B 
[21, 26, 42] float:  0.001516 byte: A8 B5 C6 3A 
[21, 27, 42] float: -0.004966 byte: CF BA A2 BB 
[21, 28, 42] float: -0.003286 byte: F6 5B 57 BB 
[21, 29, 42] float: -0.001012 byte: 6C AC 84 BA 
[21, 30, 42] float: -0.003596 byte: B6 AF 6B BB 
[22,  1, 42] float:  0.010462 byte: F0 67 2B 3C 
[22,  2, 42] float:  0.006806 byte: CC 05 DF 3B 
[22,  3, 42] float: -0.000487 byte: 4E 3D FF B9 
[22,  4, 42] float: -0.005583 byte: D9 F3 B6 BB 
[22,  5, 42] float: -0.006033 byte: F5 B0 C5 BB 
[22,  6, 42] float:  0.000798 byte: CC 44 51 3A 
[22,  7, 42] float:  0.004255 byte: AC 70 8B 3B 
[22,  8, 42] float:  0.003082 byte: 94 FF 49 3B 
[22,  9, 42] float:  0.001784 byte: FA D0 E9 3A 
[22, 10, 42] float: -0.001869 byte: 4C F4 F4 BA 
[22, 11, 42] float: -0.004282 byte: 03 4C 8C BB 
[22, 12, 42] float: -0.005237 byte: 7D 9D AB BB 
[22, 13, 42] float: -0.005017 byte: 58 66 A4 BB 
[22, 14, 42] float: -0.003749 byte: BF B6 75 BB 
[22, 15, 42] float: -0.003712 byte: B7 3D 73 BB 
[22, 16, 42] float: -0.003781 byte: 12 C9 77 BB 
[22, 17, 42] float: -0.004313 byte: F2 54 8D BB 
[22, 18, 42] float: -0.004309 byte: DC 2F 8D BB 
[22, 19, 42] float: -0.003548 byte: 37 83 68 BB 
[22, 20, 42] float: -0.001286 byte: 38 85 A8 BA 
[22, 21, 42] float:  0.002130 byte: F0 98 0B 3B 
[22, 22, 42] float:  0.005684 byte: 10 3F BA 3B 
[22, 23, 42] float:  0.008576 byte: CC 82 0C 3C 
[22, 24, 42] float:  0.008117 byte: 05 FE 04 3C 
[22, 25, 42] float:  0.002588 byte: B4 95 29 3B 
[22, 26, 42] float: -0.003819 byte: 8E 47 7A BB 
[22, 27, 42] float: -0.005202 byte: 11 78 AA BB 
[22, 28, 42] float: -0.001421 byte: FA 37 BA BA 
[22, 29, 42] float:  0.000998 byte: 70 CE 82 3A 
[22, 30, 42] float:  0.000542 byte: F0 17 0E 3A 
[23,  1, 42] float:  0.011754 byte: BA 95 40 3C 
[23,  2, 42] float:  0.011008 byte: 33 5C 34 3C 
[23,  3, 42] float:  0.001021 byte: 6B D4 85 3A 
[23,  4, 42] float: -0.007720 byte: 9F FA FC BB 
[23,  5, 42] float: -0.009272 byte: 97 EB 17 BC 
[23,  6, 42] float: -0.001939 byte: 44 36 FE BA 
[23,  7, 42] float:  0.004149 byte: 74 F1 87 3B 
[23,  8, 42] float:  0.005410 byte: 2C 44 B1 3B 
[23,  9, 42] float:  0.004818 byte: 3B DF 9D 3B 
[23, 10, 42] float:  0.000874 byte: A8 19 65 3A 
[23, 11, 42] float: -0.000828 byte: 90 F5 58 BA 
[23, 12, 42] float: -0.004069 byte: D0 54 85 BB 
[23, 13, 42] float: -0.003719 byte: F3 B5 73 BB 
[23, 14, 42] float: -0.005222 byte: D8 19 AB BB 
[23, 15, 42] float: -0.003116 byte: 0F 36 4C BB 
[23, 16, 42] float: -0.004620 byte: 9B 63 97 BB 
[23, 17, 42] float: -0.001964 byte: 6A B3 00 BB 
[23, 18, 42] float: -0.002842 byte: 34 3A 3A BB 
[23, 19, 42] float:  0.001170 byte: 04 4F 99 3A 
[23, 20, 42] float:  0.002335 byte: 80 FE 18 3B 
[23, 21, 42] float:  0.007094 byte: 8A 78 E8 3B 
[23, 22, 42] float:  0.008576 byte: CC 82 0C 3C 
[23, 23, 42] float:  0.010234 byte: F0 AC 27 3C 
[23, 24, 42] float:  0.006162 byte: C2 EB C9 3B 
[23, 25, 42] float: -0.001480 byte: A4 FE C1 BA 
[23, 26, 42] float: -0.007679 byte: 1B A3 FB BB 
[23, 27, 42] float: -0.009691 byte: 7B C8 1E BC 
[23, 28, 42] float: -0.000590 byte: 4C C5 1A BA 
[23, 29, 42] float:  0.007239 byte: 8F 31 ED 3B 
[23, 30, 42] float:  0.005035 byte: F3 FF A4 3B 
[24,  1, 42] float:  0.013257 byte: 68 33 59 3C 
[24,  2, 42] float:  0.008840 byte: F3 D6 10 3C 
[24,  3, 42] float:  0.002416 byte: 8A 56 1E 3B 
[24,  4, 42] float: -0.001890 byte: 62 AE F7 BA 
[24,  5, 42] float: -0.009022 byte: D1 D0 13 BC 
[24,  6, 42] float: -0.006652 byte: B0 F9 D9 BB 
[24,  7, 42] float: -0.001087 byte: 18 88 8E BA 
[24,  8, 42] float:  0.001075 byte: 77 D7 8C 3A 
[24,  9, 42] float:  0.004483 byte: FA E3 92 3B 
[24, 10, 42] float:  0.003620 byte: 5A 36 6D 3B 
[24, 11, 42] float:  0.002530 byte: D8 C7 25 3B 
[24, 12, 42] float:  0.001558 byte: 1B 27 CC 3A 
[24, 13, 42] float:  0.000403 byte: 3C 68 D3 39 
[24, 14, 42] float:  0.000520 byte: DC 39 08 3A 
[24, 15, 42] float:  0.000447 byte: 00 22 EA 39 
[24, 16, 42] float:  0.001559 byte: E0 67 CC 3A 
[24, 17, 42] float:  0.002274 byte: 39 FF 14 3B 
[24, 18, 42] float:  0.003934 byte: D7 E7 80 3B 
[24, 19, 42] float:  0.005795 byte: 06 E5 BD 3B 
[24, 20, 42] float:  0.007279 byte: 54 82 EE 3B 
[24, 21, 42] float:  0.009060 byte: 6C 72 14 3C 
[24, 22, 42] float:  0.008117 byte: 05 FE 04 3C 
[24, 23, 42] float:  0.006162 byte: C2 EB C9 3B 
[24, 24, 42] float: -0.001188 byte: E0 C4 9B BA 
[24, 25, 42] float: -0.008013 byte: A6 4A 03 BC 
[24, 26, 42] float: -0.012360 byte: FB 80 4A BC 
[24, 27, 42] float: -0.008894 byte: CB B8 11 BC 
[24, 28, 42] float:  0.004413 byte: B8 99 90 3B 
[24, 29, 42] float:  0.011462 byte: D0 CA 3B 3C 
[24, 30, 42] float:  0.007994 byte: AC F7 02 3C 
[25,  1, 42] float:  0.010656 byte: E9 96 2E 3C 
[25,  2, 42] float:  0.010955 byte: DA 7B 33 3C 
[25,  3, 42] float:  0.001571 byte: E4 EE CD 3A 
[25,  4, 42] float: -0.002989 byte: 92 E4 43 BB 
[25,  5, 42] float: -0.004664 byte: 37 D8 98 BB 
[25,  6, 42] float: -0.007266 byte: 5E 17 EE BB 
[25,  7, 42] float: -0.002979 byte: 22 41 43 BB 
[25,  8, 42] float: -0.002546 byte: 89 D9 26 BB 
[25,  9, 42] float: -0.000207 byte: 70 90 59 B9 
[25, 10, 42] float:  0.001236 byte: 14 04 A2 3A 
[25, 11, 42] float:  0.001712 byte: 51 5B E0 3A 
[25, 12, 42] float:  0.002296 byte: F9 76 16 3B 
[25, 13, 42] float:  0.002569 byte: CE 5C 28 3B 
[25, 14, 42] float:  0.003500 byte: C6 65 65 3B 
[25, 15, 42] float:  0.003735 byte: CC C6 74 3B 
[25, 16, 42] float:  0.005194 byte: 32 33 AA 3B 
[25, 17, 42] float:  0.005374 byte: 54 15 B0 3B 
[25, 18, 42] float:  0.006693 byte: AA 4F DB 3B 
[25, 19, 42] float:  0.006785 byte: D6 52 DE 3B 
[25, 20, 42] float:  0.006719 byte: A4 2B DC 3B 
[25, 21, 42] float:  0.005826 byte: 9E E5 BE 3B 
[25, 22, 42] float:  0.002588 byte: B4 95 29 3B 
[25, 23, 42] float: -0.001480 byte: A4 FE C1 BA 
[25, 24, 42] float: -0.008013 byte: A6 4A 03 BC 
[25, 25, 42] float: -0.010976 byte: 43 D6 33 BC 
[25, 26, 42] float: -0.013108 byte: 06 C2 56 BC 
[25, 27, 42] float: -0.004295 byte: 0A C1 8C BB 
[25, 28, 42] float:  0.006385 byte: 8D 36 D1 3B 
[25, 29, 42] float:  0.011774 byte: E1 E8 40 3C 
[25, 30, 42] float:  0.011259 byte: E9 77 38 3C 
[26,  1, 42] float:  0.006643 byte: 15 B0 D9 3B 
[26,  2, 42] float:  0.005889 byte: 8A FB C0 3B 
[26,  3, 42] float:  0.004310 byte: 88 3D 8D 3B 
[26,  4, 42] float:  0.002448 byte: E2 67 20 3B 
[26,  5, 42] float: -0.005369 byte: CE EB AF BB 
[26,  6, 42] float: -0.008325 byte: 66 64 08 BC 
[26,  7, 42] float: -0.007078 byte: B3 ED E7 BB 
[26,  8, 42] float: -0.004499 byte: EA 69 93 BB 
[26,  9, 42] float: -0.001960 byte: 40 73 00 BB 
[26, 10, 42] float: -0.000228 byte: 30 8C 6F B9 
[26, 11, 42] float:  0.001917 byte: FC 3D FB 3A 
[26, 12, 42] float:  0.002678 byte: 0C 85 2F 3B 
[26, 13, 42] float:  0.003977 byte: D6 4E 82 3B 
[26, 14, 42] float:  0.004972 byte: D0 EB A2 3B 
[26, 15, 42] float:  0.005709 byte: 5C 0F BB 3B 
[26, 16, 42] float:  0.006703 byte: FE A0 DB 3B 
[26, 17, 42] float:  0.006812 byte: 70 33 DF 3B 
[26, 18, 42] float:  0.007021 byte: BA 12 E6 3B 
[26, 19, 42] float:  0.006053 byte: 2C 58 C6 3B 
[26, 20, 42] float:  0.004157 byte: 5A 34 88 3B 
[26, 21, 42] float:  0.001516 byte: A8 B5 C6 3A 
[26, 22, 42] float: -0.003819 byte: 8E 47 7A BB 
[26, 23, 42] float: -0.007679 byte: 1B A3 FB BB 
[26, 24, 42] float: -0.012360 byte: FB 80 4A BC 
[26, 25, 42] float: -0.013108 byte: 06 C2 56 BC 
[26, 26, 42] float: -0.007919 byte: 1A BD 01 BC 
[26, 27, 42] float: -0.001957 byte: B8 3B 00 BB 
[26, 28, 42] float:  0.006749 byte: C0 26 DD 3B 
[26, 29, 42] float:  0.012892 byte: 08 37 53 3C 
[26, 30, 42] float:  0.010964 byte: 59 A0 33 3C 
[27,  1, 42] float: -0.000860 byte: 70 59 61 BA 
[27,  2, 42] float:  0.000762 byte: C0 D7 47 3A 
[27,  3, 42] float:  0.000887 byte: 42 A2 68 3A 
[27,  4, 42] float:  0.002929 byte: 9E F3 3F 3B 
[27,  5, 42] float:  0.003949 byte: F6 68 81 3B 
[27,  6, 42] float: -0.005379 byte: 7C 45 B0 BB 
[27,  7, 42] float: -0.006507 byte: 6F 35 D5 BB 
[27,  8, 42] float: -0.005308 byte: E9 EA AD BB 
[27,  9, 42] float: -0.004260 byte: 2A 94 8B BB 
[27, 10, 42] float:  0.000739 byte: 90 9C 41 3A 
[27, 11, 42] float: -0.000081 byte: 00 75 A9 B8 
[27, 12, 42] float:  0.004770 byte: 21 4A 9C 3B 
[27, 13, 42] float:  0.002435 byte: 97 8C 1F 3B 
[27, 14, 42] float:  0.006428 byte: AE 9E D2 3B 
[27, 15, 42] float:  0.003733 byte: E0 A4 74 3B 
[27, 16, 42] float:  0.006960 byte: DF 0E E4 3B 
[27, 17, 42] float:  0.003583 byte: 62 CD 6A 3B 
[27, 18, 42] float:  0.005972 byte: 36 B3 C3 3B 
[27, 19, 42] float:  0.000851 byte: FC FB 5E 3A 
[27, 20, 42] float:  0.001975 byte: BC 6E 01 3B 
[27, 21, 42] float: -0.004966 byte: CF BA A2 BB 
[27, 22, 42] float: -0.005202 byte: 11 78 AA BB 
[27, 23, 42] float: -0.009691 byte: 7B C8 1E BC 
[27, 24, 42] float: -0.008894 byte: CB B8 11 BC 
[27, 25, 42] float: -0.004295 byte: 0A C1 8C BB 
[27, 26, 42] float: -0.001957 byte: B8 3B 00 BB 
[27, 27, 42] float:  0.004167 byte: 86 8C 88 3B 
[27, 28, 42] float:  0.003564 byte: DC 92 69 3B 
[27, 29, 42] float:  0.005129 byte: E9 11 A8 3B 
[27, 30, 42] float:  0.008719 byte: FC D9 0E 3C 
[28,  1, 42] float: -0.007448 byte: BC 0A F4 BB 
[28,  2, 42] float: -0.003862 byte: AC 15 7D BB 
[28,  3, 42] float: -0.002863 byte: 99 A4 3B BB 
[28,  4, 42] float: -0.001003 byte: 6C 6E 83 BA 
[28,  5, 42] float:  0.006189 byte: C4 C9 CA 3B 
[28,  6, 42] float:  0.005556 byte: 27 11 B6 3B 
[28,  7, 42] float:  0.002145 byte: 62 98 0C 3B 
[28,  8, 42] float:  0.001800 byte: 1D FA EB 3A 
[28,  9, 42] float: -0.000160 byte: 20 70 27 B9 
[28, 10, 42] float:  0.000674 byte: 4E A1 30 3A 
[28, 11, 42] float:  0.000411 byte: F8 7F D7 39 
[28, 12, 42] float:  0.000242 byte: B0 2B 7E 39 
[28, 13, 42] float:  0.000680 byte: 0B 3E 32 3A 
[28, 14, 42] float: -0.000572 byte: 27 0D 16 BA 
[28, 15, 42] float:  0.000232 byte: 12 BC 73 39 
[28, 16, 42] float: -0.001451 byte: 3E 34 BE BA 
[28, 17, 42] float: -0.000689 byte: D7 7C 34 BA 
[28, 18, 42] float: -0.002403 byte: 72 83 1D BB 
[28, 19, 42] float: -0.002167 byte: CC 06 0E BB 
[28, 20, 42] float: -0.003080 byte: 34 DB 49 BB 
[28, 21, 42] float: -0.003286 byte: F6 5B 57 BB 
[28, 22, 42] float: -0.001421 byte: FA 37 BA BA 
[28, 23, 42] float: -0.000590 byte: 4C C5 1A BA 
[28, 24, 42] float:  0.004413 byte: B8 99 90 3B 
[28, 25, 42] float:  0.006385 byte: 8D 36 D1 3B 
[28, 26, 42] float:  0.006749 byte: C0 26 DD 3B 
[28, 27, 42] float:  0.003564 byte: DC 92 69 3B 
[28, 28, 42] float: -0.003158 byte: B3 F7 4E BB 
[28, 29, 42] float: -0.000255 byte: 30 F2 85 B9 
[28, 30, 42] float:  0.003078 byte: AC B1 49 3B 
[29,  1, 42] float: -0.012320 byte: 51 DB 49 BC 
[29,  2, 42] float: -0.013751 byte: 16 4C 61 BC 
[29,  3, 42] float: -0.002500 byte: 38 D8 23 BB 
[29,  4, 42] float:  0.002955 byte: 2E AF 41 3B 
[29,  5, 42] float:  0.005755 byte: B1 92 BC 3B 
[29,  6, 42] float:  0.013622 byte: B1 2E 5F 3C 
[29,  7, 42] float:  0.010163 byte: BE 80 26 3C 
[29,  8, 42] float:  0.008216 byte: 9C 9A 06 3C 
[29,  9, 42] float:  0.004661 byte: 0C BE 98 3B 
[29, 10, 42] float: -0.000619 byte: C8 52 22 BA 
[29, 11, 42] float: -0.001302 byte: 9C B6 AA BA 
[29, 12, 42] float: -0.006336 byte: 16 A0 CF BB 
[29, 13, 42] float: -0.005209 byte: 25 B1 AA BB 
[29, 14, 42] float: -0.009274 byte: 6E F3 17 BC 
[29, 15, 42] float: -0.007351 byte: A8 DE F0 BB 
[29, 16, 42] float: -0.010564 byte: EA 12 2D BC 
[29, 17, 42] float: -0.007888 byte: FC 3D 01 BC 
[29, 18, 42] float: -0.010288 byte: 66 90 28 BC 
[29, 19, 42] float: -0.006213 byte: 81 96 CB BB 
[29, 20, 42] float: -0.007079 byte: 6A F4 E7 BB 
[29, 21, 42] float: -0.001012 byte: 6C AC 84 BA 
[29, 22, 42] float:  0.000998 byte: 70 CE 82 3A 
[29, 23, 42] float:  0.007239 byte: 8F 31 ED 3B 
[29, 24, 42] float:  0.011462 byte: D0 CA 3B 3C 
[29, 25, 42] float:  0.011774 byte: E1 E8 40 3C 
[29, 26, 42] float:  0.012892 byte: 08 37 53 3C 
[29, 27, 42] float:  0.005129 byte: E9 11 A8 3B 
[29, 28, 42] float: -0.000255 byte: 30 F2 85 B9 
[29, 29, 42] float: -0.006291 byte: A9 24 CE BB 
[29, 30, 42] float: -0.007632 byte: C6 13 FA BB 
[30,  1, 42] float: -0.011438 byte: A6 64 3B BC 
[30,  2, 42] float: -0.013208 byte: 39 67 58 BC 
[30,  3, 42] float: -0.004257 byte: CE 7F 8B BB 
[30,  4, 42] float:  0.005250 byte: B4 0B AC 3B 
[30,  5, 42] float:  0.009131 byte: D8 98 15 3C 
[30,  6, 42] float:  0.010741 byte: 70 F9 2F 3C 
[30,  7, 42] float:  0.011326 byte: EC 8F 39 3C 
[30,  8, 42] float:  0.006572 byte: CC 56 D7 3B 
[30,  9, 42] float:  0.003172 byte: F4 E2 4F 3B 
[30, 10, 42] float: -0.001260 byte: BC 1B A5 BA 
[30, 11, 42] float: -0.005074 byte: 34 44 A6 BB 
[30, 12, 42] float: -0.006739 byte: 1D D5 DC BB 
[30, 13, 42] float: -0.009683 byte: 68 A5 1E BC 
[30, 14, 42] float: -0.009383 byte: AC BB 19 BC 
[30, 15, 42] float: -0.011537 byte: 8F 04 3D BC 
[30, 16, 42] float: -0.010210 byte: 44 49 27 BC 
[30, 17, 42] float: -0.011573 byte: A4 9C 3D BC 
[30, 18, 42] float: -0.009387 byte: 3D CA 19 BC 
[30, 19, 42] float: -0.009347 byte: E8 23 19 BC 
[30, 20, 42] float: -0.006017 byte: C6 28 C5 BB 
[30, 21, 42] float: -0.003596 byte: B6 AF 6B BB 
[30, 22, 42] float:  0.000542 byte: F0 17 0E 3A 
[30, 23, 42] float:  0.005035 byte: F3 FF A4 3B 
[30, 24, 42] float:  0.007994 byte: AC F7 02 3C 
[30, 25, 42] float:  0.011259 byte: E9 77 38 3C 
[30, 26, 42] float:  0.010964 byte: 59 A0 33 3C 
[30, 27, 42] float:  0.008719 byte: FC D9 0E 3C 
[30, 28, 42] float:  0.003078 byte: AC B1 49 3B 
[30, 29, 42] float: -0.007632 byte: C6 13 FA BB 
[30, 30, 42] float: -0.014223 byte: 4F 09 69 BC
'''

# Call the visualization function with your data strings
visualize_3d_fields(data_str_1, data_str_2)