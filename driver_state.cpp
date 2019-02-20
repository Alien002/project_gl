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
    state.image_color=0;
    state.image_depth=0;
    //std::cout<<"TODO: allocate and initialize state.image_color and state.image_depth."<<std::endl;
    
    unsigned int total_pixel = width * height;           //check, might be okay with unsigned int (pixel)
    state.image_color = new pixel[total_pixel];
    
    for(size_t i = 0; i < total_pixel; i++){
        
        state.image_color[i] = make_pixel(0,0,0);
        
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
    std::cout<<"TODO: implement rendering."<<std::endl;
    
    auto *triangle = new data_geometry[3];
    auto ptr = state.vertex_data;
    data_vertex in;
    
    
    switch (type) {
        case render_type::triangle:
            std::cout<<"render_type triangle \n";
            for(size_t i = 0, j = 0; i < state.num_vertices; i++, j++){
                triangle[i].data = ptr;
                in.data = ptr;
                state.vertex_shader(in, triangle[i], state.uniform_data);
                if(j == 2){
                    rasterize_triangle(state, (const data_geometry**) &triangle);
                    j = 0;
                }
                ptr += state.floats_per_vertex;
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
    std::cout<<"TODO: implement rasterization"<<std::endl;
    //i = w/2 * x + w/2 - 1/2
    //j = h/2 * y + h/2 - 1/2
    /*
    for(unsigned int a = 0; a < 3; a++){
        int i = (state.image_width / 2.0) * (*in)[a].gl_Position[0] + ((state.image_width / 2.0) - 0.5);
        int j = (state.image_height / 2.0) * (*in)[a].gl_Position[1] + ((state.image_height / 2.0) - 0.5);
        state.image_color[i + j * state.image_width] = make_pixel(255, 255, 255);
        
    }
    */
    //std::cout<<"TODO: implement rasterization"<<std::endl;

}

