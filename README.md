# imgy - command line image resizing tool

imgy is my C-based command line tool to provide a simple, effective way to resize images through the command line. This is a tool with developers in mind. 

**libpjeg notes**

## Example usage (as of now):
- Compiling

	\>`make`

	If you have access/use `make` on your system, simply utilize the included makefile in this repo.
- Running the program and real usage

	\>`./bin/imgy <path/to/file>`

### End goal example usage:
Example usage as follows:

	> imgy /pictures/KipNaked.png 600x800
	KipNaked.png has been resized to 600x800.

We could add some flags in there:

	> imgy /pics/CuteBinky.jpg 400x400 -o BinkyResized.jpg
	Made a resized copy of CuteBinky.jpg at /pics/BinkyResized.jpg
	
	> imgy /pics/CuteBinky.jpg 400x400 -c
	Made a resized copy of CuteBinky.jpg (CuteBinky1.jpg)  
  
How will we be doing the resizing?
	Initially we will be testing using only jpegs, but safe to assume for now that pngs will follow a similar process
	To resize the image, we are doing a bunch of black magic C stuff that I intend to understand more as this project progresses.
	First, we will be creating a struct with *jpeg_decompress_struct* hold things related to the image such as a pointer to the actual file, header data about the dimensions of the image, and some error configuration so we can properly resolve issues related to processing the image.
	Then, we will begin decompressing the image and reading the pixel lines of the image. Now, with access to pixel data, we will be able to load it up in a buffer then which we can start doing some resizing magic from.

# The big picture
Really, our program revolves around a few things:
- The decompression struct provided by libjpeg
- Our big ole buffer (stores all of our pixel data!)
- An algorithm to manipulate this buffer of pixel data into our new image (a.k.a. re-sizing)

We are creating a few structs with libjpeg, and loading them up with metadata. We provide our decompression struct with a pointer to our open file.

We then read details about the image like its header data, to understand things like the dimensions of the image.

Then, we start the decompression process.
We create a variable that represents the length of a row of pixels in our image 
We create a buffer that will be capable of holding all the pixels in our image. This is calculated by the length times the height times the components (components = 3 for RGB).
## High-level code overview
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
## Documenting Progress
Making progress
	So far, I have created a function to read a JPEG. This initializes a struct (decompression struct) with jpeg_create_decompress. I have some basic error handling that will let you know if our code found the file you provided. Then I start the decompression process.	
	Next we need to create a buffer and write the output of the decompression process to this buffer. 

I have made the full buffer by using:

	`full_buffer = malloc(decomp.output_width * decomp.output_height * decomp.output_components);` 

So we are allocating a section in memory with `malloc` that is the size of the width *times* the *height* times the *output components (3 = RGB)*. This buffer will store the entire RGB data of our image.
	
Also, we are creating an array of pointers to the rows of our image:

	 `JSAMPARRAY row_pointers = malloc(decomp.output_height * sizeof(unsigned char *));`

 We then use a for loop to allocate all the memory addresses (*pointers*) by doing some black-magic pointer arithmetic. This allows us to allocate our row pointers in the proper distance away from each other, in our test image's case would be 10080 bytes apart.
 The reason we have an array of row pointers is to have an easy way to point to every row in our image. Inside of our while loop, that utilizes jpeg_read_scanlines(), we provide a row of our buffer to read pixel data into (i.e. we provided `&row_pointers(i)`)

As briefly touched on in the previous note, we have a while loop that actually handles allocating the pixel data into our buffer:

    `int rows_read = jpeg_read_scanlines(&decomp, &row_pointers[decomp.output_scanline], 1);`

We have to realize that full_buffer is a POINTER to an ARRAY of bytes. 
`full_buffer[0]` will refer to our FIRST byte in our allocated buffer to store
a provided image.

So, in this context- know that full_buffer is an ADDRESS. the way pointer math works
is that when we add an int to an address, it will represent the address 'x' amount of 
bytes away from the original address. (where x = `(i * row_stride)`)
So we are practically saying hey, make this element in our row_pointers array to be the address of `full_buffer` `x` amount of bytes away.

