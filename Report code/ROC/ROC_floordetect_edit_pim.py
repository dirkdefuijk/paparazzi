from PIL import Image
import numpy as np
import matplotlib.pyplot as plt
import sys
import warnings

if sys.version_info[0] < 3:
    warnings.warn("This script should run using Python 3, which is currently not the case. The plot might not generate correctly.")

path = './ROC_dataset/'


def green_filter(im, param):
    """ This filter is a simple color filter around green.

    :param im: image to be filtered
    :param param: filter parameter that will be varied between 0.0 and 1.0
    :return: filtered image where detected objects have a value of [255,255,255]
    """
    # Set up the filter based on the input parameter
    # TODO: convert to YUV
    filter_width = np.array([0, 0, 0]) + param*255 # allowable range
    color_green = np.array([0, 128, 0])

    w, h = im.size

    # Load pixel data
    im_pixels = np.asarray(im.getdata(), dtype=int)

    # Create mask for detected floor: these are the pixel indices at which the filter applies
    mask = np.all(im_pixels > color_green - filter_width, axis=1) * np.all(im_pixels < color_green + filter_width,
                                                                            axis=1)
    print("mask.shape",mask.shape)
    # Create image where floor is white
    filtered_im = Image.new('RGB', (w, h), color=(0, 0, 0))
    filtered_im_pixels = np.asarray(filtered_im.getdata())
    filtered_im_pixels[mask] = [255, 255, 255]
    filtered_im.putdata([tuple(pix) for pix in filtered_im_pixels])

    return filtered_im



def RGB2YUV( rgb ):
     
    m = np.array([[ 0.29900, -0.16874,  0.50000],
                 [0.58700, -0.33126, -0.41869],
                 [ 0.11400, 0.50000, -0.08131]])
     
    yuv = np.dot(rgb,m)
    yuv[:,:,1:]+=128.0
    return yuv

def YUV2RGB( yuv ):
      
    m = np.array([[ 1.0, 1.0, 1.0],
                 [-0.000007154783816076815, -0.3441331386566162, 1.7720025777816772],
                 [ 1.4019975662231445, -0.7141380310058594 , 0.00001542569043522235] ])
    
    rgb = np.dot(yuv,m)
    rgb[:,:,0]-=179.45477266423404
    rgb[:,:,1]+=135.45870971679688
    rgb[:,:,2]-=226.8183044444304
    return rgb

def green_filter_yuv(im, param):
    """ This filter is a simple color filter around green.

    :param im: image to be filtered
    :param param: filter parameter that will be varied between 0.0 and 1.0
    :return: filtered image where detected objects have a value of [255,255,255]
    """
    # Set up the filter based on the input parameter
    # TODO: convert to YUV
    
    
    slide = np.array([param*255, 0, 0]) # allowable range
    color_green_upper = np.array([140, 150, 180])     #80 is the bandwidth
    color_green_lower = np.array([140, 0, 0]) 

    w, h = im.size

    # Load pixel data
    im_pixels = np.asarray(im.getdata(), dtype=int)
    
    im_pixels = im_pixels.reshape(240,520,3)
    
    # plt.imshow(im_pixels, interpolation='nearest')
    # plt.show()
    
    # print("im_pixels enkele voor",im_pixels[10][10])
    # print("im_pixels voor",im_pixels.shape)
    im_pixels = RGB2YUV(im)
    # print("im_pixels na",im_pixels.shape)
    # print("im_pixels enkele na",im_pixels[10][10])
    
    
    im_pixels = im_pixels.reshape(124800,3)
    #print("im_pixels na na",im_pixels.shape)
    # Create mask for detected floor: these are the pixel indices at which the filter applies
    #mask = np.all(im_pixels < color_green_upper + slide, axis=1) * np.all(im_pixels > color_green_lower + slide, axis=1)
    mask = np.all(im_pixels < color_green_upper + slide, axis=1) * np.all(im_pixels > color_green_lower - slide, axis=1)
    #print(mask.shape)
    
    #print("mask.shape",mask.shape)
    # Create image where floor is white
    filtered_im = Image.new('RGB', (w, h), color=(0, 0, 0))
    filtered_im_pixels = np.asarray(filtered_im.getdata())
    filtered_im_pixels[mask] = [255, 255, 255]
    filtered_im.putdata([tuple(pix) for pix in filtered_im_pixels])
    
    plt.imshow(filtered_im, interpolation='nearest')
    plt.show()

    return filtered_im




def generate_ROC_plot():
    """ Generates a simple ROC plot"""
    plot_data = []
    n_images = 5    # Number of images in folder
    for param in np.linspace(0.1, 0.3, 20):
        # Initialize totals
        true_positives = 0
        false_positives = 0
        ground_truth_positives = 0
        ground_truth_negatives = 0

        for i in range(1, n_images + 1):                                    #in range should be from 1 but now 2 to check other images
            # Set image paths
            original_path = path + 'original/' + str(i) + '_original.jpg'
            ground_truth_path = path + 'mask/' + str(i) + '_mask.png'

            # Analyze ground truth image
            ground_truth_im = Image.open(ground_truth_path, 'r')
            ground_truth_pixels = np.asarray(ground_truth_im.getdata())
            ground_truth_floor = np.all(ground_truth_pixels == [255, 255, 255], axis=1)

            # Analyze original image
            im = Image.open(original_path, 'r')
            filtered_im = green_filter_yuv(im, param)       
            #filtered_im = green_filter(im, param)                                                #change from rgb to yuv version
            filtered_im_pixels = np.asarray(filtered_im.getdata())
            filtered_im_floor = np.all(filtered_im_pixels == [255, 255, 255], axis=1)


            # Update totals of positives/negatives
            true_positives += np.sum((filtered_im_floor == True) & (ground_truth_floor == True))
            false_positives += np.sum((filtered_im_floor == True) & (ground_truth_floor == False))

            ground_truth_positives += np.sum((ground_truth_floor == True))
            ground_truth_negatives += np.sum((ground_truth_floor == False))

        # Calculate rates
        false_positive_rate = false_positives / ground_truth_negatives
        true_positive_rate = true_positives / ground_truth_positives

        # Add datapoint to plot_data
        plot_data.append((false_positive_rate, true_positive_rate))

    # Create x and y data from plot_data
    x = [item[0] for item in plot_data]
    y = [item[1] for item in plot_data]

    print(f"x = {x}\ny = {y}")

    # Plot
    plt.plot(x, y)
    plt.xlabel("False Positive Rate")
    plt.ylabel("True Positive Rate")
    plt.xlim(0, 1)
    plt.ylim(0, 1)
    plt.grid()
    plt.show()


# Main script
generate_ROC_plot()



