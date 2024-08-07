//
// Created by Smugg on 01.08.2024.
//
#include "JNIConverter.h"
#include <vector>
#include <cstdlib>
#include "heif.h"
#include "jpeglib.h"
#include "iostream"

using namespace std;

vector<uint8_t> heiicToJPG(const vector<uint8_t>& heicData) {
    vector<uint8_t> jpgData;


    heif_context* ctx = heif_context_alloc();
    if (!ctx) {
        cerr << "Error: Failed to allocate HEIF context" << endl;
        return jpgData;
    }
    cout << "heif context allocated successfully" << endl;


    heif_error error = heif_context_read_from_memory(ctx, heicData.data(), heicData.size(), nullptr);
    if (error.code != 0) {
        cerr << "Error: Failed to read heic data from memory, code: " << error.code << endl;
        heif_context_free(ctx);
        return jpgData;
    }
    cout << "heic data read successfully" << endl;


    heif_image_handle* handle;
    error = heif_context_get_primary_image_handle(ctx, &handle);
    if (error.code != 0 || !handle) {
        cerr << "Error: Failed to get primary image handle, code: " << error.code << endl;
        heif_context_free(ctx);
        return jpgData;
    }
    cout << "Primary image handle obtained successfully" << endl;


    heif_image* img;
    error = heif_decode_image(handle, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGB, nullptr);
    if (error.code != 0 || !img) {
        cerr << "Error: Failed to decode image, code: " << error.code << endl;
        heif_image_handle_release(handle);
        heif_context_free(ctx);
        return jpgData;
    }
    cout << "Image decoded successfully" << endl;


    int width = heif_image_get_width(img, heif_channel_interleaved);
    int height = heif_image_get_height(img, heif_channel_interleaved);
    const uint8_t* data = heif_image_get_plane_readonly(img, heif_channel_interleaved, nullptr);

    if (!data) {
        cerr << "Error: Failed to get image plane data" << endl;
        heif_image_release(img);
        heif_image_handle_release(handle);
        heif_context_free(ctx);
        return jpgData;
    }

    cout << "Image width: " << width << ", height: " << height << endl;


    jpeg_compress_struct cinfo;
    jpeg_error_mgr jpgerr;
    cinfo.err = jpeg_std_error(&jpgerr);
    jpeg_create_compress(&cinfo);

    unsigned char* jpegBuffer = nullptr;
    unsigned long jpegSize = 0;
    jpeg_mem_dest(&cinfo, &jpegBuffer, &jpegSize);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = const_cast<uint8_t*>(&data[cinfo.next_scanline * width * 3]);
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    cout << "jpg compression finished" << endl;


    jpeg_finish_compress(&cinfo);
    jpgData.assign(jpegBuffer, jpegBuffer + jpegSize);

    jpeg_destroy_compress(&cinfo);
    heif_image_release(img);
    heif_image_handle_release(handle);
    heif_context_free(ctx);


    cout << "End Of Conversion" << endl;
    cout << "jpg data size: " << jpgData.size() << " bytes" << endl;

    return jpgData;
}

extern "C"
JNIEXPORT jbyteArray JNICALL Java_org_converter_JNIConverter_Converter(JNIEnv* env, jobject obj, jbyteArray heicArray) {
    jsize length = env->GetArrayLength(heicArray);
    cout << "Received heic data of length: " << length << endl;
    vector<uint8_t> heicData(length);

    env->GetByteArrayRegion(heicArray, 0, length, reinterpret_cast<jbyte*>(heicData.data()));

    vector<uint8_t> jpegData = heiicToJPG(heicData);
    cout << "Converted jpg data length: " << jpegData.size() << endl;

    if (jpegData.empty()) {
        cerr << "Error: jpg data is empty" << endl;
        return nullptr;
    }

    jbyteArray jpegArray = env->NewByteArray(jpegData.size());
    env->SetByteArrayRegion(jpegArray, 0, jpegData.size(), reinterpret_cast<const jbyte*>(jpegData.data()));
    return jpegArray;
}