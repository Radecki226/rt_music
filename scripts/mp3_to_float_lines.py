#!/usr/bin/env python3
"""
Decode an MP3 to a text file with one IEEE float sample per line.

Uses the PyPI ``miniaudio`` package (dr_mp3 inside the wheel) — **no ffmpeg**.

  pip install miniaudio

Samples are float32, written as decimal text one value per line (interleaved if
multi-channel: L,R,L,R,...).

Example:
  ./scripts/mp3_to_float_lines.py clip.mp3 clip_samples.txt
  ./scripts/mp3_to_float_lines.py clip.mp3 out.txt --channels 2 --sample-rate 44100
"""

from __future__ import annotations

import argparse
import array
import sys
from pathlib import Path


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("input_mp3", type=Path, help="Input .mp3 path")
    p.add_argument(
        "output_txt",
        type=Path,
        nargs="?",
        help="Output text file (default: <input>.float_lines.txt)",
    )
    p.add_argument(
        "--channels",
        type=int,
        default=1,
        metavar="N",
        help="Output audio channels (default: 1 mono). Stereo => interleaved L,R,...",
    )
    p.add_argument(
        "--sample-rate",
        type=int,
        default=None,
        metavar="HZ",
        help="Resample to this rate (Hz). Omit to keep the file's native sample rate.",
    )
    return p.parse_args()


def float_lines_from_mp3(
    mp3_path: Path,
    out_path: Path,
    *,
    channels: int,
    sample_rate: int | None,
) -> None:
    if channels < 1:
        raise SystemExit("--channels must be >= 1")

    try:
        import miniaudio
    except ImportError as exc:
        raise SystemExit(
            "Missing dependency: pip install miniaudio\n"
            "(Uses embedded MP3 decode; ffmpeg is not required.)"
        ) from exc

    dec = miniaudio.mp3_read_file_f32(str(mp3_path.resolve()))
    target_sr = sample_rate if sample_rate is not None else dec.sample_rate
    target_ch = channels

    if target_sr == dec.sample_rate and target_ch == dec.nchannels:
        pcm_f32 = dec.samples
    else:
        raw = miniaudio.convert_frames(
            miniaudio.SampleFormat.FLOAT32,
            dec.nchannels,
            dec.sample_rate,
            dec.samples.tobytes(),
            miniaudio.SampleFormat.FLOAT32,
            target_ch,
            target_sr,
        )
        pcm_f32 = array.array("f")
        pcm_f32.frombytes(bytes(raw))

    out_path.parent.mkdir(parents=True, exist_ok=True)
    with out_path.open("w", encoding="utf-8") as fout:
        for sample in pcm_f32:
            fout.write(f"{sample:.18g}\n")


def main() -> None:
    args = parse_args()
    inp = args.input_mp3
    if not inp.is_file():
        raise SystemExit(f"Input not found: {inp}")

    out = args.output_txt
    if out is None:
        out = inp.with_suffix(".float_lines.txt")

    float_lines_from_mp3(
        inp,
        out,
        channels=args.channels,
        sample_rate=args.sample_rate,
    )
    print(f"Wrote {out}", file=sys.stderr)


if __name__ == "__main__":
    main()
