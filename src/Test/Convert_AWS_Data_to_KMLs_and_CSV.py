import tkinter as tk
from tkinter import filedialog
import csv
import base64
import simplekml

def select_file():
    """Open a file dialog to select a .csv or .txt file and process it."""
    root = tk.Tk()
    root.withdraw()  # Hide the main window

    file_path = filedialog.askopenfilename(
        title="Select AWS Log File",
        filetypes=[
            ("CSV files", "*.csv"),
            ("Text files", "*.txt"),
            ("All files", "*.*")
        ]
    )
    
    if file_path:
        process_file(file_path)
    else:
        print("No file selected.")
    
    root.destroy()

def sidewalk_payload_to_hex_string(sidewalk_payload):
    """
    Converts an AWS Sidewalk payload (Base64 encoded) to a string of 2-digit
    hexadecimal characters, where the decoded bytes are treated as an ASCII
    string and each pair of ASCII characters is combined into a hex byte,
    separated by spaces.

    Args:
        sidewalk_payload (str): The Sidewalk payload string from the input file.

    Returns:
        str: A string of 2-digit hexadecimal bytes separated by spaces,
             or None if decoding fails.
    """
    try:
        # Decode the Base64 payload to bytes
        binary_data = base64.b64decode(sidewalk_payload)
        # Convert bytes to ASCII string
        ascii_string = binary_data.decode('ascii')
        # Combine each pair of ASCII characters into a hex byte
        hex_bytes = [f'{int(ascii_string[i:i+2], 16):02x}' for i in range(0, len(ascii_string), 2)]
        # Join with spaces
        hex_string = ' '.join(hex_bytes)
        return hex_string
    except Exception as e:
        print(f"Error processing payload: {e}")
        return None

def generate_kml_files(csv_path):
    """Generate KML files from the CSV based on Payload Content values with custom suffixes and dot icons."""
    # Initialize KML objects
    kml_01 = simplekml.Kml(name="Payload Content 01 - BLE")
    kml_02 = simplekml.Kml(name="Payload Content 02 - FSK")
    kml_03 = simplekml.Kml(name="Payload Content 03 - CSS")

    # Define styles for each KML with dot icons
    # Yellow dot for 01 (BLE)
    style_01 = simplekml.Style()
    style_01.iconstyle.color = simplekml.Color.yellow
    style_01.iconstyle.scale = 1.0  # Smaller scale for a dot-like appearance
    style_01.iconstyle.icon.href = 'http://maps.google.com/mapfiles/kml/shapes/shaded_dot.png'

    # Cyan dot for 02 (FSK)
    style_02 = simplekml.Style()
    style_02.iconstyle.color = simplekml.Color.cyan
    style_02.iconstyle.scale = 1.0
    style_02.iconstyle.icon.href = 'http://maps.google.com/mapfiles/kml/shapes/shaded_dot.png'

    # Red dot for 03 (CSS)
    style_03 = simplekml.Style()
    style_03.iconstyle.color = simplekml.Color.red
    style_03.iconstyle.scale = 1.0
    style_03.iconstyle.icon.href = 'http://maps.google.com/mapfiles/kml/shapes/shaded_dot.png'

    # Counters to track if data exists
    has_01 = False
    has_02 = False
    has_03 = False

    # Read the CSV
    with open(csv_path, 'r', encoding='utf-8') as csvfile:
        reader = csv.reader(csvfile)
        headers = next(reader)  # Skip header row
        for row in reader:
            try:
                payload_content = row[4]  # Column E (index 4)
                latitude = float(row[5])  # Column F (index 5)
                longitude = float(row[6]) # Column G (index 6)
                timestamp = row[11]       # Column L (Timestamp, now index 11 after J deletion)

                # Create points based on Payload Content
                if payload_content == "01":
                    point = kml_01.newpoint(name=timestamp, coords=[(longitude, latitude)])                    
                    point.name = None  # Remove the name
                    point.description = None # Remove the description (if any was set)
                    point.style = style_01
                    has_01 = True
                elif payload_content == "02":
                    point = kml_02.newpoint(name=timestamp, coords=[(longitude, latitude)])
                    point.name = None  # Remove the name
                    point.description = None # Remove the description (if any was set)
                    point.style = style_02
                    has_02 = True
                elif payload_content == "03":
                    point = kml_03.newpoint(name=timestamp, coords=[(longitude, latitude)])
                    point.name = None  # Remove the name
                    point.description = None # Remove the description (if any was set)
                    point.style = style_03
                    has_03 = True
            except (ValueError, IndexError) as e:
                print(f"Error processing row {row}: {e}")
                continue

    # Save KML files with custom suffixes only if they have data
    base_name = csv_path.rsplit('.', 1)[0]
    if has_01:
        kml_01.save(f"{base_name}_BLE.kml")
        print(f"KML file saved: {base_name}_BLE.kml")
    if has_02:
        kml_02.save(f"{base_name}_FSK.kml")
        print(f"KML file saved: {base_name}_FSK.kml")
    if has_03:
        kml_03.save(f"{base_name}_CSS.kml")
        print(f"KML file saved: {base_name}_CSS.kml")

def process_file(file_path):
    """Process the file: remove quotes, braces, strings, split by commas, convert PayloadData to hex, add new columns, delete first data row, and delete column J in final CSV."""
    output_csv = file_path.rsplit('.', 1)[0] + '_processed.csv'
    
    # Strings to remove
    strings_to_remove = [
        'MessageId:', 'WirelessDeviceId:', 'PayloadData:', 'WirelessMetadata:', 
        'Sidewalk:', 'CmdExStatus:', 'MessageType:', 'NackExStatus:', 
        'SidewalkId:', 'Timestamp:'
    ]
    
    # Original custom headers with Date/Time first
    custom_headers = [
        'Date/Time', 'MessageId:', 'WirelessDeviceId:', 'PayloadData:', 
        'WirelessMetadata:', 'Msg Type:', ' ', 'Seq Number:', 
        'SidewalkId:', 'Timestamp:', 'Rule:'
    ]
    
    # Insert new columns after PayloadData (column D)
    new_headers = ['Payload Content', 'Latitude', 'Longitude']
    fieldnames = custom_headers[:4] + new_headers + custom_headers[4:]
    
    with open(file_path, 'r', encoding='utf-8') as infile, \
         open(output_csv, 'w', newline='', encoding='utf-8') as outfile:
        
        # Read all lines to determine the maximum number of columns and process initial data
        lines = []
        max_columns = 0
        for line in infile:
            # Remove all double quotes
            line_no_quotes = line.replace('"', '')
            # Remove braces
            line_no_braces = line_no_quotes.replace('{', '').replace('}', '')
            # Remove specified strings
            processed_line = line_no_braces
            for string in strings_to_remove:
                processed_line = processed_line.replace(string, '')
            # Split by commas
            columns = processed_line.strip().split(',')
            # Convert PayloadData (column 4, index 3) from Base64 to hex with combined pairs
            if len(columns) >= 4:
                hex_output = sidewalk_payload_to_hex_string(columns[3])
                columns[3] = hex_output if hex_output is not None else columns[3]
            lines.append(columns)
            max_columns = max(max_columns, len(columns))
        
        # Adjust fieldnames based on max columns, ensuring new headers are included
        if max_columns <= len(custom_headers):
            fieldnames = fieldnames[:max_columns + 3]  # +3 for new columns
        else:
            fieldnames = fieldnames + [f'Column_{i+1}' for i in range(len(custom_headers), max_columns)]
        
        # Process lines to add new columns
        processed_lines = []
        for columns in lines:
            # Extract hex bytes from PayloadData (column 3 after initial processing)
            if len(columns) >= 4 and columns[3]:
                hex_bytes = columns[3].split()
                # Payload Content: array[0]
                payload_content = hex_bytes[0] if len(hex_bytes) > 0 else ''
                
                # Latitude: signed int32 from (array[1]<<24) + (array[2]<<16) + (array[3]<<8) + array[4], divided by 1000000
                if len(hex_bytes) >= 5:
                    lat_int = (int(hex_bytes[1], 16) << 24) + \
                              (int(hex_bytes[2], 16) << 16) + \
                              (int(hex_bytes[3], 16) << 8) + \
                              int(hex_bytes[4], 16)
                    # Convert to signed 32-bit integer
                    if lat_int & (1 << 31):  # Check sign bit
                        lat_int -= (1 << 32)
                    latitude = float(lat_int) / 1000000
                else:
                    latitude = ''
                
                # Longitude: signed int32 from (array[5]<<24) + (array[6]<<16) + (array[7]<<8) + array[8], divided by 1000000
                if len(hex_bytes) >= 9:
                    lon_int = (int(hex_bytes[5], 16) << 24) + \
                              (int(hex_bytes[6], 16) << 16) + \
                              (int(hex_bytes[7], 16) << 8) + \
                              int(hex_bytes[8], 16)
                    # Convert to signed 32-bit integer
                    if lon_int & (1 << 31):  # Check sign bit
                        lon_int -= (1 << 32)
                    longitude = float(lon_int) / 1000000
                else:
                    longitude = ''
                
                # Insert new columns after PayloadData (index 3)
                new_columns = [payload_content, latitude, longitude]
                processed_columns = columns[:4] + new_columns + columns[4:]
            else:
                # If PayloadData is missing or invalid, insert empty columns
                processed_columns = columns[:4] + ['', '', ''] + columns[4:]
            
            processed_lines.append(processed_columns)
        
        # Remove the first data row (index 0) if there is at least 1 row
        if processed_lines:
            processed_lines.pop(0)
        
        # Delete column J (index 9, 'SidewalkId:') from header and all rows
        fieldnames = fieldnames[:9] + fieldnames[10:]  # Remove index 9
        final_lines = []
        for line in processed_lines:
            final_lines.append(line[:9] + line[10:])  # Remove index 9 from each row
        
        # Write to CSV
        writer = csv.writer(outfile)
        writer.writerow(fieldnames)  # Write updated header without column J
        
        # Write each line's columns
        for columns in final_lines:
            try:
                writer.writerow(columns)
            except Exception as e:
                print(f"Error processing line: {','.join(map(str, columns))}. Error: {e}")
                continue
    
    print(f"Processed file saved as: {output_csv}")
    
    # Generate KML files from the processed CSV
    generate_kml_files(output_csv)

if __name__ == "__main__":
    select_file()