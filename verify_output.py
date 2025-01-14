import struct
import os
import sys

def read_binary_file(file_path):
    """Reads a binary video file and returns its contents."""
    try:
        with open(file_path, "rb") as f:
            no_of_frames = struct.unpack("q", f.read(8))[0]
            channels = struct.unpack("B", f.read(1))[0]
            height = struct.unpack("B", f.read(1))[0]
            width = struct.unpack("B", f.read(1))[0]
            frame_size = channels * height * width
            frames = [f.read(frame_size) for _ in range(no_of_frames)]
            return {
                "no_of_frames": no_of_frames,
                "channels": channels,
                "height": height,
                "width": width,
                "frames": frames,
            }
    except FileNotFoundError:
        print(f"Error: File not found: {file_path}")
        return None
    except struct.error:
        print(f"Error: Invalid file format or structure error in {file_path}")
        return None
    except IOError:
        print(f"Error: I/O error while reading {file_path}")
        return None

def compare_files(input_path, output_path, operation, params=None):
    """Compares input and output video files after a given operation."""
    input_data = read_binary_file(input_path)
    output_data = read_binary_file(output_path)

    if input_data is None or output_data is None:
        return False

    if not (input_data["no_of_frames"] == output_data["no_of_frames"] and
            input_data["channels"] == output_data["channels"] and
            input_data["height"] == output_data["height"] and
            input_data["width"] == output_data["width"]):
        print("Error: Video metadata mismatch.")
        return False

    if operation == "reverse":
        for i in range(input_data["no_of_frames"]):
            if input_data["frames"][i] != output_data["frames"][-(i + 1)]:
                print(f"Frame {i} does not match the reverse of the output frame {input_data['no_of_frames'] - 1 - i}")
                return False
        print("Reverse operation verified successfully!")
        return True

    elif operation == "swap_channel":
        channel1, channel2 = params
        for frame_index, (in_frame, out_frame) in enumerate(zip(input_data["frames"], output_data["frames"])):
            input_frame = bytearray(in_frame)
            output_frame = bytearray(out_frame)
            for i in range(len(input_frame) // input_data["channels"]):
                idx1 = i * input_data["channels"] + channel1
                idx2 = i * input_data["channels"] + channel2
                if input_frame[idx1] != output_frame[idx2] or input_frame[idx2] != output_frame[idx1]:
                    print(f"Frame {frame_index}, Pixel {i}: Channel swap mismatch.")
                    return False
        print("Swap channel operation verified successfully!")
        return True

    elif operation == "clip_channel":
        channel, min_val, max_val = params
        for frame_index, frame in enumerate(output_data["frames"]):
            for i in range(channel, len(frame), output_data["channels"]):
                if not (min_val <= frame[i] <= max_val):
                    print(f"Frame {frame_index}, Pixel {i // output_data['channels']}: Value {frame[i]} out of range.")
                    return False
        print("Clip channel operation verified successfully!")
        return True

    elif operation == "scale_channel":
        channel, scale_factor = params
        for frame_index, (in_frame, out_frame) in enumerate(zip(input_data["frames"], output_data["frames"])):
            input_frame = bytearray(in_frame)
            output_frame = bytearray(out_frame)
            for i in range(channel, len(input_frame), input_data["channels"]):
                expected_value = min(255, max(0, int(input_frame[i] * scale_factor)))
                if output_frame[i] != expected_value:
                    print(f"Frame {frame_index}, Pixel {i // input_data['channels']}: Expected {expected_value}, Found {output_frame[i]}")
                    return False
        print("Scale channel operation verified successfully!")
        return True

    else:
        print("Unknown operation, cannot verify.")
        return False

if __name__ == "__main__":
    try:
        print("Testing reverse functionality...")
        result_reverse = compare_files("test.bin", "output_reverse.bin", "reverse")
        print("Reverse test:", "Passed" if result_reverse else "Failed")

        print("\nTesting swap_channel functionality...")
        result_swap = compare_files("test.bin", "output_swap.bin", "swap_channel", [0, 2])
        print("Swap channel test:", "Passed" if result_swap else "Failed")

        print("\nTesting clip_channel functionality...")
        result_clip = compare_files("test.bin", "output_clip.bin", "clip_channel", [1, 10, 200])
        print("Clip channel test:", "Passed" if result_clip else "Failed")

        print("\nTesting scale_channel functionality...")
        result_scale = compare_files("test.bin", "output_scale.bin", "scale_channel", [1, 1.5])
        print("Scale channel test:", "Passed" if result_scale else "Failed")

    except Exception as e:
        print(f"Error: {e}")