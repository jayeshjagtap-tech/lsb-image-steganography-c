# LSB Image Steganography in C

This project implements a simple image steganography technique using the Least Significant Bit (LSB) method in C.  
The program hides secret text inside a BMP image and can later extract the hidden message without affecting the visible quality of the image.

The main objective of this project is to understand how information can be concealed within digital images using bit manipulation and basic image processing concepts.

---

## Project Overview

Steganography is a technique used to hide information inside another file so that the presence of the hidden data is not obvious.

In this project, the least significant bits of the pixel values in a BMP image are modified to store secret text. Since only the last bit of each pixel byte is changed, the visual appearance of the image remains almost the same.

The program performs two main operations:

• Encoding – hiding secret text inside an image  
• Decoding – extracting the hidden text from the stego image

---

## Technologies Used

C Programming Language  
GCC Compiler  
File Handling in C  
Bitwise Operations  
Basic Image Processing Concepts

---

## Project Structure

LSB-Image-Steganography

encode.c  
decode.c  
encode.h  
decode.h  
test_encode.c  
test_decode.c  
types.h  
common.h  
secret.txt  
beautiful.bmp  
README.md

---

## File Description

encode.c  
Contains the implementation of the encoding process where secret data is embedded into the image.

decode.c  
Contains the implementation of the decoding process where hidden data is extracted from the image.

encode.h / decode.h  
Header files containing function declarations used for encoding and decoding.

test_encode.c  
Main driver file used to run the encoding process.

test_decode.c  
Main driver file used to run the decoding process.

types.h  
Defines user-defined data types used across the project.

common.h  
Contains shared definitions such as the magic string used to detect hidden data.

secret.txt  
Sample file that contains the message to be hidden.

beautiful.bmp  
Sample BMP image used as the cover image.

---

## How the Program Works

Encoding Process

1. The program reads the BMP image file.
2. The secret text from the input file is converted into binary form.
3. Each bit of the message is embedded into the least significant bit of the image pixel data.
4. A new image called the stego image is created which contains the hidden message.

Decoding Process

1. The program reads the stego image.
2. It checks for a predefined magic string to confirm that hidden data exists.
3. The least significant bits from the image pixels are extracted.
4. These bits are combined to reconstruct the original secret message.

---

## How to Compile the Project

Compile the encoder program:

gcc test_encode.c encode.c -o encode

Compile the decoder program:

gcc test_decode.c decode.c -o decode

---

## How to Run the Program

Encoding (hide message inside image)

./encode -e beautiful.bmp secret.txt stego.bmp

This command hides the contents of secret.txt inside the image and creates a new image called stego.bmp.

Decoding (extract hidden message)

./decode -d stego.bmp recovered.txt

This command extracts the hidden message from stego.bmp and stores it in recovered.txt.

---

## Example

Content of secret.txt

My password is SECRET ;)

After running the encoder, the message will be hidden inside stego.bmp.  
Running the decoder will retrieve the same message and store it in recovered.txt.

---

## Learning Outcome

Working on this project helped me understand:

• How BMP images store pixel data  
• How bitwise operations can be used for data hiding  
• How steganography works in practice  
• Handling binary files and data manipulation in C

---

## Conclusion

This project demonstrates how secret data can be embedded inside an image using the Least Significant Bit technique. The hidden message does not cause any visible change in the image, making it an effective way to conceal information. The implementation also helped in understanding file handling, bit manipulation, and basic image processing in C.

## Author

Jayesh Jagtap
