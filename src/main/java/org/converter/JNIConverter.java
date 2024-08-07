package org.converter;

import java.io.*;

import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;


public class JNIConverter {
    static {
        try {
            loadLibraryFromJar("/native/libde265.dll");
            loadLibraryFromJar("/native/libx265.dll");
            loadLibraryFromJar("/native/heif.dll");
            loadLibraryFromJar("/native/turbojpeg.dll");
            loadLibraryFromJar("/native/jpeg62.dll");
            loadLibraryFromJar("/native/libjni_converter.dll");

        }
        catch (Exception e) {
            System.err.println("Error loading native libraries:" + e.getMessage());
            System.exit(1);
        }
    }
    private static void loadLibraryFromJar(String path) throws IOException {
        String libName = path.substring(path.lastIndexOf('/') + 1);
        InputStream in = JNIConverter.class.getResourceAsStream(path);
        if (in == null) {
            throw new RuntimeException("Native library " + libName + " not found in JAR");
        }
        Path tempDir = Files.createTempDirectory("nativeLib");
        Path tempLib = tempDir.resolve(libName);
        Files.copy(in, tempLib, StandardCopyOption.REPLACE_EXISTING);
        in.close();
        System.out.println("Loading native library from: " + tempLib);
        System.load(tempLib.toString());
        System.out.println("Native library loaded successfully: " + libName);
    }

    public native byte[] Converter(byte[] files);

    public static void main(String[] args) {

        JNIConverter converter = new JNIConverter();

        if (args.length < 2) {
            System.err.println("java org.converter.JNIConverter error. No output file name specified ");
            System.exit(1);
        }


        String inputFilePath = args[0];
        String outputFilePath = args[1];


        byte[] heicData;

        try (FileInputStream fis = new FileInputStream(inputFilePath)) {
            heicData = new byte[(int) new File(inputFilePath).length()];
            int bytes_read= fis.read(heicData);

            System.out.println("Read heic data successfully, length: " + bytes_read);
        } catch (IOException e) {

            System.err.println("Error reading file: " + inputFilePath);
            System.exit(1);
            return;
        }

        byte[] jpegData = converter.Converter(heicData);

        try {
            if (jpegData == null || jpegData.length == 0) {
                throw new RuntimeException("Conversion returned null or empty byte[]");
            }
            System.out.println("Conversion successful, jpg data length: " + jpegData.length);
        } catch (Exception e) {
            System.err.println("Conversion failed: " + e.getMessage());
            System.exit(1);
            return;
        }

        try (FileOutputStream fos = new FileOutputStream(outputFilePath)) {
            fos.write(jpegData);
            System.out.println("jpg data written to: " + outputFilePath);
        } catch (IOException e) {
            System.err.println("Error writing file: " + outputFilePath);
            System.exit(1);
        }
    }
}
