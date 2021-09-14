/**
 * @file filler.cpp
 * Implementation of functions in the filler namespace.
 *
 */

/**
 * Performs a multi-point flood fill using breadth first search.
 *
 * @param config      FillerConfig struct to setup the fill
 * @return animation  object illustrating progression of flood fill algorithm
 */
animation filler::fillBFS(FillerConfig &config)
{
    return fill<Queue>(config);
}

/**
 * Performs a multi-point flood fill using depth first search.
 *
 * @param config      FillerConfig struct to setup the fill
 * @return animation  object illustrating progression of flood fill algorithm
 */
animation filler::fillDFS(FillerConfig &config)
{
    return fill<Stack>(config);
}

/**
 * Run a multi-point flood fill on an image
 *
 * @param  config     FillerConfig struct with data for flood fill of image
 * @return animation  object illustrating progression of flood fill algorithm
 */
template <template <class T> class OrderingStructure> animation filler::fill(FillerConfig &config)
{
    /**
     * @todo You need to implement this function!
     *
     * This is the basic description of a flood-fill algorithm: Every fill
     * algorithm requires an ordering structure, which is passed to this
     * function via its template parameter. For a breadth-first-search
     * fill, that structure is a Queue, for a depth-first-search, that
     * structure is a Stack. To begin the algorithm, you simply place the
     * given point in the ordering structure, marking it processed
     * (the way you mark it is a design decision you'll make yourself).
     * We have a choice to either change the color, if appropriate, when we
     * add the point to the OS, or when we take it off. In our test cases,
     * we have assumed that you will change the color when a point is added
     * to the structure.
     * Until the structure is empty, you do the following:
     *
     * 1.     Remove a point from the ordering structure, and then...
     *
     *        1.    add its unprocessed neighbors whose color values are
     *              within (or equal to) tolerance distance from the center,
     *              to the ordering structure, and
     *              mark them as processed.
     *        2.    if it is an appropriate frame, send the current PNG to the
     *              animation (as described below).
     *
     * 2.     When implementing your breadth-first-search and
     *        depth-first-search fills, you will need to explore neighboring
     *        pixels in some order.
     *
     *        While the order in which you examine neighbors does not matter
     *        for a proper fill, you must use the same order as we do for
     *        your animations to come out like ours! The order you should put
     *        neighboring pixels **ONTO** the queue or stack is as follows:
     *        ** UP(-y), RIGHT(+x), DOWN(+y), LEFT(-x). IMPORTANT NOTE: *UP*
     *        here means towards the top of the image, so since an image has
     *        smaller y coordinates at the top, this is in the *negative y*
     *        direction. Similarly, *DOWN* means in the *positive y*
     *        direction.** To reiterate, when you are exploring (filling out)
     *        from a given pixel, you must first try to fill the pixel UP
     *        from it, then the one to its RIGHT, then DOWN from it and
     *        finally to its LEFT. If you do them in a different order, your fill may
     *        still work correctly, but your animations will be different
     *        from the grading scripts!
     *
     *        Because we are doing a multi-point flood fill you will repeat the flood
     *        fill from several locations stored in the `centres` variable of FillerConfig
     *        struct. Initial fill points should be visited in the order they stored in the
     *        `centres` vector. If a pixel lies in the fill region of multiple start points,
     *        it should only be updated by the first fill. To repeat, pixels should only
     *        be modified the first time they are visited by the filler algorithm.
     *
     * 3.     For every k pixels filled, **starting at the kth pixel**, you
     *        must add a frame to the animation, where k = frameFreq.
     *
     *        For example, if frameFreq is 4, then after the 4th pixel has
     *        been filled you should add a frame to the animation, then again
     *        after the 8th pixel, etc.  You must only add frames for the
     *        number of pixels that have been filled, not the number that
     *        have been checked. So if frameFreq is set to 1, a pixel should
     *        be filled every frame.
     *
     * 4.     Finally, as you leave the function, send one last frame to the
     *        animation. This frame will be the final result of the fill, and
     *        it will be the one we test against.
     *
     */
    // Create new animation
    animation myAnimation;
    OrderingStructure<point> orderingStruct;

    // Get variables from config and store locally
    int frameFreq = config.frameFreq;
    float tolerance = config.tolerance;
    PNG img = config.img;
    vector<centre> centres = config.centres;
    vector<ColorPicker *> pickers = config.pickers;

    // Set counter for the number of frames created
    int frames = frameFreq;

    // Get image width and height and store into variables
    int imgWidth = (int)img.width();
    int imgHeight = (int)img.height(); 

    // Create 2D boolean vector to track which pixels have been processed, set all to false
    vector<vector<bool>> processedPixels;
    processedPixels.resize(imgHeight);

    for (int i = 0; i < imgHeight; i++) {
        processedPixels[i].resize(imgWidth);

        for (int j = 0; j < imgWidth; j++) {
            processedPixels.at(i).at(j) = false;
        }
    }

    // Add initial untouched frame
    myAnimation.addFrame(img);

    // Iterate through every center
    for (int centerCounter = 0; centerCounter < (int)centres.size(); centerCounter++) {

        // Decrement frames
        frames--;

        // Get the right color picker
        ColorPicker* colorPicker = pickers.at(centerCounter);

        // Get the center based on index and add it to the ordering structure
        centre c = centres.at(centerCounter);
        orderingStruct.add(point(c));

        // Confirm frameFreq and frame count is divisible and add filler frames as needed
        int temp_frames = frames;

        // Add filler frame to fit up with frameFreq
        if (frames == 0) {
            myAnimation.addFrame(img);
            frames = frameFreq;
        }

        // Go through every point in orderingStruct until none remaining
        while (!orderingStruct.isEmpty()) {

            // Remove top point from the structure
            point p = orderingStruct.remove();

            // Get coordinates for pixels at each of the four locations
            int up = p.y - 1;
            int right = p.x + 1;
            int down = p.y + 1;
            int left = p.x - 1;

            // If up is within borders, process pixel
            if (isWithinImage(p.x,up,imgWidth,imgHeight)) {

                HSLAPixel* pixel = img.getPixel(p.x, up);
                point temp = point(p.x, up, c);

                if (!processedPixels.at(up).at(p.x) && c.color.dist(*pixel) <= tolerance) {

                    // Decrement frame count
                    frames--;

                    // Process pixel in the colorPicker functor
                    *pixel = colorPicker->operator()(temp);

                    // Add new pixel to structure to process
                    orderingStruct.add(temp);

                    // Set pixel to processed
                    processedPixels.at(up).at(p.x) = true;

                    // Add the image to the animation
                    if (frames == 0) {
                        myAnimation.addFrame(img);
                        frames = frameFreq;
                    }
                }
            }

            // If right is within borders, process pixel
            if (isWithinImage(right,p.y,imgWidth,imgHeight)) {

                HSLAPixel* pixel = img.getPixel(right, p.y);
                point temp = point(right, p.y, c);

                if (!processedPixels.at(p.y).at(right) && c.color.dist(*pixel) <= tolerance) {

                    // Decrement frame count
                    frames--;

                    // Process pixel in the colorPicker functor
                    *pixel = colorPicker->operator()(temp);

                    // Add new pixel to structure to process
                    orderingStruct.add(temp);

                    // Set pixel to processed
                    processedPixels.at(p.y).at(right) = true;

                    // Add the image to the animation
                    if (frames == 0) {
                        myAnimation.addFrame(img);
                        frames = frameFreq;
                    }
                }
            }

            // If down is within borders, process pixel
            if (isWithinImage(p.x,down,imgWidth,imgHeight)) {

                HSLAPixel* pixel = img.getPixel(p.x, down);
                point temp = point(p.x, down, c);

                if (!processedPixels.at(down).at(p.x) && c.color.dist(*pixel) <= tolerance) {

                    // Decrement frame count
                    frames--;

                    // Process pixel in the colorPicker functor
                    *pixel = colorPicker->operator()(temp);

                    // Add new pixel to structure to process
                    orderingStruct.add(temp);

                    // Set pixel to processed
                    processedPixels.at(down).at(p.x) = true;

                    // Add the image to the animation
                    if (frames == 0) {
                        myAnimation.addFrame(img);
                        frames = frameFreq;
                    }
                }
            }

            // If left is within borders, process pixel
            if (isWithinImage(left,p.y,imgWidth,imgHeight)) {

                HSLAPixel* pixel = img.getPixel(left, p.y);
                point temp = point(left, p.y, c);

                if (!processedPixels.at(p.y).at(left) && c.color.dist(*pixel) <= tolerance) {

                    // Decrement frame count
                    frames--;

                    // Process pixel in the colorPicker functor
                    *pixel = colorPicker->operator()(temp);

                    // Add new pixel to structure to process
                    orderingStruct.add(temp);

                    // Set pixel to processed
                    processedPixels.at(p.y).at(left) = true;

                    // Add the image to the animation
                    if (frames == 0) {
                        myAnimation.addFrame(img);
                        frames = frameFreq;
                    }
                }
            }
        }
    }


    // Delete memory used by 2D vector
    for (int i = 0; i < imgHeight; i++) {
        processedPixels.at(i).clear();
        processedPixels.at(i).shrink_to_fit();
    }
    processedPixels.clear();
    processedPixels.shrink_to_fit();

    // Add final image frame to animation and return animation
    myAnimation.addFrame(img);
    return myAnimation;
}

bool filler::isWithinImage(int x, int y, int width, int height) 
{
    return (x >= 0 && x < width && y >= 0 && y < height);
}
