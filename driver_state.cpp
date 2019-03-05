#include "driver_state.h"
#include <cstring>

driver_state::driver_state()
{
}

driver_state::~driver_state()
{
    delete [] image_color;
    delete [] image_depth;
}

// This function should allocate and initialize the arrays that store color and
// depth.  This is not done during the constructor since the width and height
// are not known when this class is constructed.
void initialize_render(driver_state& state, int width, int height)
{
    state.image_width=width;
    state.image_height=height;
    state.image_color=nullptr;
    state.image_depth=nullptr;
    //std::cout<<"TODO: allocate and initialize state.image_color and state.image_depth."<<std::endl;
    
    unsigned long long total_pixel = width * height;           //check, might be okay with unsigned int (pixel)
    state.image_color = new pixel[total_pixel];
    state.image_depth = new float[total_pixel];
    
    for(size_t i = 0; i < total_pixel; ++i){
        
        state.image_color[i] = make_pixel(0,0,0);
        state.image_depth[i] = 2;
    }
    
}

// This function will be called to render the data that has been stored in this class.
// Valid values of type are:
//   render_type::triangle - Each group of three vertices corresponds to a triangle.
//   render_type::indexed -  Each group of three indices in index_data corresponds
//                           to a triangle.  These numbers are indices into vertex_data.
//   render_type::fan -      The vertices are to be interpreted as a triangle fan.
//   render_type::strip -    The vertices are to be interpreted as a triangle strip.
void render(driver_state& state, render_type type)
{
    //std::cout<<"TODO: implement rendering."<<std::endl;
    
    data_geometry *triangle = new data_geometry[3];
    float *ptr = state.vertex_data;
    data_vertex in{};
    
    
    switch (type) {
        case render_type::triangle:
            std::cout<<"render_type triangle \n";
            
            for(size_t i = 0; i < (state.num_vertices/3); ++i) {
                for(unsigned j = 0; j < 3; ++j){
                    triangle[j].data = ptr;
                    ptr += state.floats_per_vertex;
                }
                
                for(unsigned k = 0; k < 3; ++k){
                    in.data = triangle[k].data;
                    state.vertex_shader(in, triangle[k], state.uniform_data);
                }
                
                rasterize_triangle(state, (const data_geometry**) &triangle);
            }

            break;
        case render_type::indexed:
            break;
        case render_type::fan:
            break;
        case render_type::strip:
            break;
        default:
            break;
    }
    
    delete [] triangle;
    
}


// This function clips a triangle (defined by the three vertices in the "in" array).
// It will be called recursively, once for each clipping face (face=0, 1, ..., 5) to
// clip against each of the clipping faces in turn.  When face=6, clip_triangle should
// simply pass the call on to rasterize_triangle.
void clip_triangle(driver_state& state, const data_geometry* in[3],int face)
{
    if(face == 6)
    {
        rasterize_triangle(state, in);
        return;
    }
    std::cout<<"TODO: implement clipping. (The current code passes the triangle through without clipping them.)"<<std::endl;
    clip_triangle(state,in,face+1);
}

// Rasterize the triangle defined by the three vertices in the "in" array.  This
// function is responsible for rasterization, interpolation of data to
// fragments, calling the fragment shader, and z-buffering.
void rasterize_triangle(driver_state& state, const data_geometry* in[3])
{
    //std::cout<<"TODO: implement rasterization"<<std::endl;
    //i = w/2 * x + w/2 - 1/2
    //j = h/2 * y + h/2 - 1/2
    int x[3], y[3];
    
    //Calculats i & j coordinates in NDC for vertices
    for(unsigned int a = 0; a < 3; ++a){
        int i = static_cast<int>((state.image_width / 2.0f)
                                 * (*in)[a].gl_Position[0]/(*in)[a].gl_Position[3]
                                 + ((state.image_width / 2.0f) - 0.5f));
        int j = static_cast<int>((state.image_height / 2.0f)
                                 * (*in)[a].gl_Position[1]/(*in)[a].gl_Position[3]
                                 + ((state.image_height / 2.0f) - 0.5f));
        x[a] = i;
        y[a] = j;
        
        //state.image_color[i + j * state.image_width] = make_pixel(255, 255, 255);
        
    }
    
    //finds the min/max of triangle
    int min_x = std::min(std::min(x[0], x[1]), x[2]);
    int max_x = std::max(std::max(x[0], x[1]), x[2]);
    int min_y = std::min(std::min(y[0], y[1]), y[2]);
    int max_y = std::max(std::max(y[0], y[1]), y[2]);
    
    
    //Makes sure triangle is within pixel grid
    if(min_x < 0){
        min_x = 0;
    }
    if(min_y < 0){
        min_y = 0;
    }
    if(max_x > state.image_width){
        max_x = state.image_width - 1;
    }
    if(max_y > state.image_height){
        max_y = state.image_height - 1;
    }
    
    //calculates area of the triangle
    float area_abc = (0.5f * ((x[1]*y[2] - x[2]*y[1]) - (x[0]*y[2] - x[2]*y[0]) + (x[0]*y[1] - x[1]*y[0])));
    
    auto *data = new float[MAX_FLOATS_PER_VERTEX];
    data_fragment fragment_data{data};
    data_output out_data;
    
    for(int j = min_y; j <= max_y; ++j){
        for(int i = min_x; i <= max_x; ++i){
            
            float alpha = (0.5f * ((x[1] * y[2] - x[2] * y[1]) + (y[1] - y[2])*i + (x[2] - x[1])*j)) / area_abc;
            float beta = (0.5f * ((x[2] * y[0] - x[0] * y[2]) + (y[2] - y[0])*i + (x[0] - x[2])*j)) / area_abc;
            float gamma = (0.5f * ((x[0] * y[1] - x[1] * y[0]) + (y[0] - y[1])*i + (x[1] - x[0])*j)) / area_abc;
        
            
            float depth = alpha * (*in)[0].gl_Position[2] + beta * (*in)[1].gl_Position[2] + gamma * (*in)[2].gl_Position[2];
            
            if(alpha >= 0 && beta >= 0 && gamma >= 0){
                const float alpha_p = alpha;
                const float beta_p = beta;
                const float gamma_p = gamma;
                /*
                if(depth1 > state.image_depth[i + j * state.image_width]){
                    depth1 = state.image_depth[i + j * state.image_width];
                }
                */
                for(int k = 0; k < state.floats_per_vertex; ++k){
                    float k_gour;
                    switch(state.interp_rules[k]){
                        case interp_type::flat:
                            
                            fragment_data.data[k] = (*in)[0].data[k];
                            
                            break;
                        case interp_type::smooth:
                            
                            k_gour = (alpha_p / (*in)[0].gl_Position[3])
                                        + (beta_p / (*in)[1].gl_Position[3])
                                        + (gamma_p / (*in)[2].gl_Position[3]);
                            
                            
                            alpha = alpha_p / (k_gour * (*in)[0].gl_Position[3]);
                            beta = beta_p / (k_gour * (*in)[1].gl_Position[3]);
                            gamma = gamma_p / (k_gour * (*in)[2].gl_Position[3]);
                            
                            //fragment_data.data[k] = alpha + beta + gamma;
                            //break;
                        case interp_type::noperspective:
                            
                            fragment_data.data[k] = alpha * (*in)[0].data[k]
                                                    + beta * (*in)[1].data[k]
                                                    + gamma * (*in)[2].data[k];
                            
                            break;
                        default:
                            break;
                            
                            
                    }
                }
                
                state.fragment_shader(fragment_data, out_data, state.uniform_data);
                state.image_color[i + j * state.image_width] = make_pixel((out_data.output_color[0] * 255),
                                                                          (out_data.output_color[1] * 255),
                                                                          (out_data.output_color[2] * 255));
                state.image_depth[i + j * state.image_width] = depth;
            }
        }
    }
    //std::cout<<"TODO: implement rasterization"<<std::endl;
    delete [] data;
}

