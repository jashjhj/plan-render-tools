#include <SFML/Graphics.hpp>
#include <cmath>

using namespace sf;

const Color grey = Color(150, 0, 0);
int canvas_x = 2000;
int canvas_y = 1414;

bool fisheye = true;

namespace render{

    // inclusive
    void fill_column(unsigned int x, unsigned int y, unsigned int y2, Image* image, Color colour){
        for (unsigned int i = y; i <= y2; i++){
            //printf("%d", colour.r);
            image->setPixel(x, i, colour);
        }
    }

    Image draw(float* vals, float arc){
        Image canvas = Image();
        canvas.create(canvas_x, canvas_y, Color(255,255,255)); // ratio 2:sqrt2 like a paper.


        unsigned int vals_len = 0;

        float sum_dist;

        printf("Initalising Render...     ");

        while (vals[vals_len] != -100.0f){
            sum_dist += (float)vals[vals_len];
            vals_len ++;
        }

        // get average distance. this should be middle length on drawing.
        float middle_dist = sum_dist / (float) vals_len;

        printf("Rendering %d points, Middle distance %f...    ", vals_len * 2, middle_dist);

        for (unsigned long int i = 0; i<vals_len; i++){
            float angle = (float)i/(float)vals_len * arc - arc/2.0f;
            float oblique_dist = cos(angle) * vals[i];
            if (fisheye){
                oblique_dist = vals[i]; //linear no test for oblique distamce.
            }


            float screen_proportion = middle_dist/oblique_dist; // Promise it works xxxx.  Returns 1 for middle dist, others scaled correctly.
            int dist_from_top = std::min(canvas_y-1, std::max(0, (int)((float)canvas_y * (0.5f-screen_proportion/4.0f))));
            int x_proportion = (int) floor((float)i / vals_len * (float)canvas_x);

            float colour_intensity = (float)dist_from_top / (float)canvas_y * 2.0f;
            Color colour_fill = Color((char)(255.0f * colour_intensity), (char)(255.0f * colour_intensity), (char)(255.0f * colour_intensity));

            fill_column(x_proportion, dist_from_top, canvas_y-1 - dist_from_top, &canvas, colour_fill);
        }
        printf("Done!\n");
        return canvas;
    }

}
