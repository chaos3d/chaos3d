#include "asset_support/png_loader.h"
#include "io/memory_stream.h"
#include <png.h>

png_loader::png_loader(data_stream* source, int format)
: _buf_size(0), _buffer(nullptr), _desc{{0, 0}, format}{
    if(source == nullptr || !source->valid())
        throw std::exception(); // TODO: fix throw
    
    assert(format == image_desc::A8 ||
           format == image_desc::RGB565 ||
           format == image_desc::RGBA8888);
    
    load(*source);
}

png_loader::~png_loader() {
    if(_buffer)
        delete [] _buffer;
    
    _buffer = nullptr;
}

class buffer_wrapper_stream : public memory_stream {
public:
    buffer_wrapper_stream(png_loader::const_ptr && loader)
    : _loader(std::move(loader)),
    memory_stream(loader->buffer(), loader->buf_size()){
        
    };
    
private:
    png_loader::const_ptr _loader;
};

std::unique_ptr<memory_stream> png_loader::data() const {
    return std::unique_ptr<memory_stream>(new buffer_wrapper_stream(retain<png_loader>()));
}

static void PNGAPI user_read_data_fcn(png_structp png_ptr, png_bytep data, png_size_t length){
    png_size_t check;

    data_stream* ds = (data_stream*)png_get_io_ptr(png_ptr);
    check = (png_size_t) ds->read((void*)data,length);

    if (check != length)
        png_error(png_ptr, "Read Error");
}

void png_loader::load(data_stream& ds) {
    //TODO: fix error loading
    
	png_byte buffer[8];
	png_bytep *row_pointers = 0;
	png_bytep data = 0;
	
	// Read the first few bytes of the PNG file
	if (ds.read(buffer, 8) != 8) {
		//LOG_STAT(ERROR,"Unable to read a PNG." );
		return;
	}
    
	// Check if it really is a PNG file
	if (png_sig_cmp(buffer, 0, 8)) {
		//LOG_STAT(ERROR,"PNG checked error");
		return;
	}
    
	png_structp png_ptr = png_create_read_struct(
                                                 PNG_LIBPNG_VER_STRING,
                                                 NULL, NULL, NULL);
    
	if (png_ptr == 0) {
		//LOG_STAT(ERROR,"Unable to create png structure.");
		return;
	}
    
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == 0) {
		//LOG_STAT(ERROR,"Unable to create png info structure.");
		png_destroy_read_struct(&png_ptr,(png_infopp)0, (png_infopp)0);
		return;
	}
    
	// for proper error handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		if (row_pointers)
			delete [] row_pointers;
		if ( data )
			delete [] data;
		//LOG_STAT(ERROR,"Reading PNG errors.");
		return;
	}
    
	png_set_read_fn(png_ptr, (png_voidp)&ds, user_read_data_fcn);
    
	png_set_sig_bytes(png_ptr, 8); // Tell png that we read the signature
    
	png_read_info(png_ptr, info_ptr); // Read the info section of the png file
    
    
	int width, height;
	int bitDepth, colorType;
	int channels = png_get_channels(png_ptr, info_ptr);
    
    {
		// Use temporary variables to avoid passing casted pointers
		png_uint_32 w,h;
		// Extract info
		png_get_IHDR(png_ptr, info_ptr,
                     &w, &h,
                     &bitDepth, &colorType, NULL, NULL, NULL);
		width = w;
		height= h;
	}
    
	// Convert palette color to true color
	if (colorType == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);
    
	// Convert low bit colors to 8 bit colors
	if (bitDepth < 8){
		if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_expand_gray_1_2_4_to_8(png_ptr);
		else
			png_set_packing(png_ptr);
	}
    
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);
    
	// Convert high bit colors to 8 bit colors
	if (bitDepth == 16)
		png_set_strip_16(png_ptr);
    
    if (_desc.format == image_desc::A8 && channels != 1) {
        // convert to grey scale and use it as an alpha mask
        // and strip the alpha channel
        png_set_strip_alpha(png_ptr);
        png_set_rgb_to_gray(png_ptr, 1, 0.2109375, 0.71484375);
    }
    
	// Update the changes
	png_read_update_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr,
                 (png_uint_32*)&width, (png_uint_32*)&height,
                 &bitDepth, &colorType, NULL, NULL, NULL);
	channels = png_get_channels(png_ptr, info_ptr);
    
    _desc.size = {width, height};
    
	// Create array of pointers to rows in image data
	row_pointers = new png_bytep[height];
    _buf_size = sizeof(png_byte) * width * height * channels;
	_buffer = new char [_buf_size];
    data = (png_bytep)_buffer;
    
	if (!row_pointers || !data) {
		//LOG_STAT(ERROR,"Unable to allocate enough memory.");
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return;
	}
    
	png_bytep rp = data;
	for ( int i = 0 ; i < height ; ++i, rp += width * channels ){
		row_pointers[i] = rp;
	}
    
	// Read data using the library function that handles all transformations including interlacing
	png_read_image(png_ptr, row_pointers);
    
    if (_desc.format == image_desc::A8 && channels != 1) {
        assert(0); // shouldn't happen?
    } else if (_desc.format == image_desc::RGB565) {
		unsigned char* tempData = new unsigned char[width * height * 2];
		unsigned char* in = data;
		unsigned short* out = (unsigned short*)tempData;
        
		for (int i = 0; i < width * height; ++i, in += channels)
			*out++ = ((unsigned short)(*in >> 3)<<11) |
            ((unsigned short)(*(in+1) >> 2)<<5) |
            ((unsigned short)(*(in+2) >> 3));
        
		delete [] data;
    }

	png_read_end(png_ptr, NULL);
	delete [] row_pointers;

	png_destroy_read_struct(&png_ptr,&info_ptr, 0); // Clean up memory
}
