# imgy - command line image resizing tool

imgy is my C-based command line tool to provide a simple, effective way to resize images through the command line. This is a tool with developers in mind. 

**libpjeg notes**

We are using libjpeg 
Example usage as follows:

	> imgy /pictures/KipNaked.png 600x800
	KipNaked.png has been resized to 600x800.

We could add some flags in there:

	> imgy /pics/CuteBinky.jpg 400x400 -o BinkyResized.jpg
	Made a resized copy of CuteBinky.jpg at /pics/BinkyResized.jpg
	
	> imgy /pics/CuteBinky.jpg 400x400 -c
	Made a resized copy of CuteBinky.jpg (CuteBinky1.jpg)  
  
How will we be taking input from the user?
```
int argc, char *argv
```

How will we be doing the resizing?
	Initially we will be testing using only jpegs, but safe to assume for now that pngs will follow a similar process
	To resize the image, we are doing a bunch of black magic C stuff that I intend to understand more as this project progresses.
	First, we will be creating a struct with *jpeg_decompress_struct* hold things related to the image such as a pointer to the actual file, header data about the dimensions of the image, and some error configuration so we can properly resolve issues related to processing the image.
	Then, we will begin decompressing the image and reading the pixel lines of the image. Now, with access to pixel data, we will be able to load it up in a buffer then which we can start doing some resizing magic from.

# High-level code overview
How should we build this project? Well, it's C. So, thank god, that we don't have to deal with stupid OOP principles. We are going to have a couple files that have a variety of functions that we will try our best to have clear, defined purposes. So what will these functions look like?

* int main(int main(int argc, char \*argv[])
	this is our main function. it will handle taking our command line arguments. we can expect to handle our input errors here, and handle supplying all the functions that actually make up our program their necessary data.

	Currently it calls:
	- read_jpeg()

- int read_jpeg(char \*infile, char \*\*outfile)
	This a sort of *main* function for getting our jpeg data. This will handle the definition and initialization of our *decomp* struct. It will pass this struct to our *do_read_jpeg* which just feels like a total badass function name out of the like 90s or something.

- int do_read_jpeg(struct jpeg_decompress_struct decomp, char \*infilepath, char \*outfilepath)
	This is where the real meat and taters is of our progress so far. This handles setting up our error struct, actually opening the image provided and reading the header data, pixel data, etc.
		

- **TO IMPLEMENT** int resize_jpeg()
	We can expect to either call this from do_read_jpeg, or, somehow handle passing the buffer to it so then it can manipulate and resize it in that function.
	This function will contain our crazy nearest neighbor algorithm for resizing.

## Example Usage (as of now)
- Compiling

	\>`make`

	If you have access/use `make` on your system, simply utilize the included makefile in this repo.

- Running the program and real usage

	\>`./bin/imgy <path/to/file>`


Making progress
	So far, I have created a function to read a JPEG. This initializes a struct (decompression struct) with jpeg_create_decompress. I have some basic error handling that will let you know if our code found the file you provided. Then I start the decompression process.	
	Next we need to create a buffer and write the output of the decompression process to this buffer.
