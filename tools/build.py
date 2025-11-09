import argparse

def build_parser():
    parser = argparse.ArgumentParser(
        description="Universal project builder CLI"
    )

    parser.add_argument("--preset")
    return parser

def main():
    parser = build_parser()

if __name__ == "__main__":
    main()

