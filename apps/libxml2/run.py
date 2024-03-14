import os
import json
import subprocess
import argparse
from datetime import datetime
from hashlib import md5
from pathlib import Path

def main(args):
    seed = args.seed
    timestamp = datetime.now().strftime("%m%d-%H%M%S")
    print(timestamp)
    fingerprint = f"s{args.seed:04}-"

    base_dir = Path.cwd() / "exp" / (fingerprint + timestamp)
    base_dir.mkdir(exist_ok=True, parents=True)

    corpus_dir = base_dir / "corpus"
    corpus_dir.mkdir(exist_ok=True, parents=True)
    corpus_dir = str(corpus_dir)

    output_dir = base_dir / "output"
    output_dir.mkdir(exist_ok=True, parents=True)
    output_dir = str(output_dir) + "/"

    subprocess.run(["unzip", "seed.zip", "-d", corpus_dir])
    # ../../afl-fuzz -p -i seed -o bzip_perf_test/ -m none -N 250 ./driver @@

    cmds = [
            "timeout",
            f"{args.timeout}s",
            "../../afl-fuzz",
            "-p",
            "-i",
            corpus_dir,
            "-o",
            output_dir,
            "-m",
            "none",
            "-N",
            f"{args.max_len}",
            "-x",
            "../../dictionaries/xml.dict",
            "./driver",
            "@@"
            ]
    print(" \\\n    ".join(cmds))

    with open(base_dir / "config.json", "w") as f:
        config = vars(args)
        config.update({
            "date": timestamp
            })
        json.dump(config, f)

    subprocess.run(cmds)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--seed", type=int, default=0)
    parser.add_argument("--timeout", type=int, default=21600)
    parser.add_argument("--max_len", type=int, default=500)

    args = parser.parse_args()
    main(args)
