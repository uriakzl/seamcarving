Seam Carving – Content-Aware Image Resizing
This project implements seam carving, a content-aware image resizing technique that removes vertical or horizontal seams of least importance from an image. Unlike traditional resizing methods such as cropping or scaling, seam carving preserves the image’s key features and structures by intelligently selecting and removing low-energy pixels. This approach allows for natural-looking resizing while maintaining important content and aspect ratios.

At the core of the algorithm is the dual-gradient energy function, which calculates the energy of each pixel based on color gradients. High-energy pixels typically represent edges or important image content, making them less likely to be removed. Using this energy map, the algorithm computes a cost array via dynamic programming to determine the optimal seam path from top to bottom. This seam is the one with the minimum total energy.

Once the seam is identified, it is removed from the image, reducing its width by one pixel. This process can be repeated multiple times to shrink the image incrementally. The implementation includes functions to compute the energy map, find the minimum energy seam, recover the path, and remove the seam to generate the resized image.

This repository contains all necessary code for implementing seam carving in C, along with example workflows to apply the technique on .bin format images. The project demonstrates how simple yet powerful algorithms can be used to perform advanced image processing tasks. Originally developed as part of a computer graphics assignment, this implementation shows how seam carving works under the hood and why it has become a standard tool in professional image editing software like Adobe Photoshop.

