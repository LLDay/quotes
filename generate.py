import os
import subprocess

proto_dir = "client/proto"
name_without_extension = "quotes"
proto_name = name_without_extension + ".proto"


def generate():
    os.makedirs(proto_dir, exist_ok=True)
    subprocess.check_call(
        f"protoc --python_out={proto_dir} {proto_name}".split())


if __name__ == "__main__":
    generate()
