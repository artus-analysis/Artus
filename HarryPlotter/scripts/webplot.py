#!/usr/bin/env python

import argparse

import Artus.HarryPlotter.webplot as webplot


def main():
	parser = argparse.ArgumentParser(description="Create web gallery for a set of plots.")

	parser.add_argument("input_dir", help="Input directory containing the plots")
	parser.add_argument("-r", "--recursive", default=False, action="store_true",
	                    help="Recursively search for plots in the input directory.")
	parser.add_argument("--file-types", default=["png", "jpg", "pdf", "svg", "eps"],
	                    help="File types to be considered as plots. [Default: %(default)s]")
	parser.add_argument("-c", "--copy", default=False, action="store_true",
	                    help="Copy plots into your www-directory. Otherwise only creates index.html files.")

	args = parser.parse_args()

	if "/" in args.input_dir[0:-1]:
		print "Please do not use '/' for the input directory."
		print "Instead, go into the directory where your directory containing the plots is located, then run webplot.py."
		print "(This is to avoid getting lots of subdirectories in your webspace)"
		exit()

	webplot.webplot(
			input_dir=args.input_dir,
			recursive=args.recursive,
			file_types=args.file_types,
			copy=args.copy
	)
	

if __name__ == "__main__":
	main()
