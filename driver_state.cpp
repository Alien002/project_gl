#include "driver_state.h"
#include <cstring>
#include <cfloat>

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
        state.image_depth[i] = 1;
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
    /*
     const data_geometry *output[3];
     data_geometry *triangle = new data_geometry[3];
     float *ptr = state.vertex_data;
     data_vertex in{};
     */
    
    //float *v1_fan = ptr;
    
    switch (type) {
        case render_type::triangle:{
            const data_geometry *output[3];

            data_geometry *triangle = new data_geometry[3];
            float *ptr = state.vertex_data;
            data_vertex in{};
            /*
            for(size_t i = 0; i < state.num_vertices; i += 3) {
                //fills the triangle vertex data
                data_geometry** triangle = new data_geometry*[3];
                
                for(unsigned j = 0; j < 3; ++j){
                    triangle[j] = new data_geometry;
                    data_vertex vertex;
                    vertex.data = new float[MAX_FLOATS_PER_VERTEX];
                    triangle[j] -> data = new float[MAX_FLOATS_PER_VERTEX];
                    
                    for(unsigned k = 0; k < state.floats_per_vertex; ++k){
                        vertex.data[k] = state.vertex_data[k+state.floats_per_vertex*(i + j)];
                        triangle[j] -> data[k] = vertex.data[k];
                        state.vertex_shader((const data_vertex)vertex, *triangle[j], state.uniform_data);

                    }
                    
                }
                clip_triangle(state, (const data_geometry**) triangle, 0);
            }
            */
            for(size_t i = 0; i < (state.num_vertices/3); ++i) {
                for(unsigned j = 0; j < 3; ++j){
                    triangle[j].data = ptr;
                    ptr += state.floats_per_vertex;
                }
                
                for(unsigned k = 0; k < 3; ++k){
                    in.data = triangle[k].data;
                    state.vertex_shader(in, triangle[k], state.uniform_data);
                    output[k] = &triangle[k];

                }
                
                //rasterize_triangle(state, (const data_geometry**) &triangle);
                clip_triangle(state, output, 0);

            }
            
            
        }
            break;
        case render_type::strip:{
            const data_geometry *output[3];
            data_geometry triangle[3];
            data_vertex vertex[3];
            
            for(size_t i = 0; i < state.num_vertices - 2; ++i) {
                
                for(unsigned j = 0; j < 3; ++j){
                    vertex[j].data = &state.vertex_data[(i + j) * state.floats_per_vertex];
                    triangle[j].data = vertex[j].data;
                    state.vertex_shader(vertex[j], triangle[j], state.uniform_data);
                    output[j] = &triangle[j];
                    
                }
                clip_triangle(state, output, 0); //maybe output instead of &triangle
                //rasterize_triangle(state, (const data_geometry**) &triangle);
            }
            
            
        }
            break;
        case render_type::fan:{
            /*
             XYZ XYZ XYZ
             XYZ     XYZ XYZ
             XYZ         XYZ XYZ
             XYZ             XYZ XYZ ...
             */
            //std::cout<<"!!render type fan!!\n";
            const data_geometry *output[3];
            data_geometry triangle[3];
            data_vertex vertex[3];
            
            int flag;
            
            // triangle[0].data = ptr; //triangle[0].data set outside for loop, wont change values
            
            for(size_t i = 0; i < state.num_vertices; ++i) {
                //std::cout<<"!!render type fan i = " <<i <<"\n";
                for(unsigned j = 0; j < 3; ++j){
                    flag = i + j;
                    if (j == 0){
                        flag = 0;
                    }
                    vertex[j].data = &state.vertex_data[flag * state.floats_per_vertex];
                    triangle[j].data = vertex[j].data;
                    state.vertex_shader(vertex[j], triangle[j], state.uniform_data);
                    output[j] = &triangle[j];
                    
                }
                
                //rasterize_triangle(state, (const data_geometry**) &triangle);
                clip_triangle(state, output, 0); //maybe output instead of &triangle
                
            }
            
        }
            break;
        case render_type::indexed:{
            const data_geometry *output[3];
            data_geometry triangle[3];
            data_vertex vertex[3];
            
            for(size_t i = 0; i < state.num_triangles * 3; i += 3){
                for(unsigned j = 0; j < 3; ++j){
                    vertex[j].data = &state.vertex_data[state.index_data[i + j] * state.floats_per_vertex];
                    triangle[j].data = vertex[j].data;
                    state.vertex_shader(vertex[j], triangle[j], state.uniform_data);
                    output[j] = &triangle[j];
                }
                clip_triangle(state, output, 0);
            }
        }
            break;
        default:
            break;
    }
    
    //delete [] triangle;
    
}




// This function clips a triangle (defined by the three vertices in the "in" array).
// It will be called recursively, once for each clipping face (face=0, 1, ..., 5) to
// clip against each of the clipping faces in turn.  When face=6, clip_triangle should
// simply pass the call on to rasterize_triangle.
void clip_triangle(driver_state& state, const data_geometry* in[3],int face)
{
    /*
    if(face == 6)
    {
        rasterize_triangle(state, in);
        return;
    }
    */
    
    if(face == 1){
        rasterize_triangle(state,in);
        return;
    }
    
    
    //std::cout<<"TODO: implement clipping. (The current code passes the triangle through without clipping them.)"<<std::endl;
    vec4 a = in[0] -> gl_Position;
    vec4 b = in[1] -> gl_Position;
    vec4 c = in[2] -> gl_Position;
    
    const data_geometry *in3[3] = {in[0], in[1], in[2]};
    
    data_geometry d1[3], d2[3];
    
    float a1, b1, b2;
    vec4 p1, p2;
    
    
    
    if(a[2] < -a[3] && b[2] < -b[3] && c[2] < -c[3]){
        return;
    }
    
    if(a[2] < -a[3] && b[2] >= -b[3] && c[2] >= -c[3]){
        b1 = (-b[3] - b[2]) / (a[2] + a[3] - b[3] - b[2]);
        b2 = (-a[3] - a[2]) / (c[2] + c[3] - a[3] - a[2]);

        p1 = b1 * a + (1 - b1) * b;
        p2 = b2 * c + (1 - b2) * a;
        
        d1[0].data = new float[state.floats_per_vertex];
        d1[1] = *in[1];
        d1[2] = *in[2];
        
        for(int i = 0; i < state.floats_per_vertex; ++i){
            
            switch (state.interp_rules[i]){
             
                case interp_type::flat:
                    
                    d1[0].data[i] = in[0]->data[i];
                    
                    break;
                case interp_type::smooth:
                    
                    d1[0].data[i] = b2* in[2]->data[i] + (1 - b2) * in[0]->data[i];
                    
                    break;
                case interp_type::noperspective:
                    
                    a1 = b2 * in[2]->gl_Position[3] / (b2 * in[2]->gl_Position[3] + (1 - b2) * in[0]->gl_Position[3]);
                    d1[0].data[i] = a1 * in[2]->data[i] + (1 - a1) * in[0]->data[i];
                    break;
                
                default:
                    break;
            }
            
        }
        
        d1[0].gl_Position = p2;
        in3[0] = &d1[0];
        in3[1] = &d1[1];
        in3[2] = &d1[2];
        
        clip_triangle(state,in3,face + 1);
        
        d2[0].data = new float[state.floats_per_vertex];
        d2[2] = *in[2];
    
        for(int i = 0; i < state.floats_per_vertex; ++i){
            
            switch (state.interp_rules[i]){
                    
                case interp_type::flat:
                    
                    d2[0].data[i] = in[0] -> data[i];
                    
                    break;
                case interp_type::smooth:
                    
                    d2[0].data[i] = b1 * in[0] -> data[i] + (1 - b1) * in[1] -> data[i];
                    
                    break;
                case interp_type::noperspective:
                    
                    a1 = b1 * in[0] -> gl_Position[3] / (b1 * in[0] -> gl_Position[3] + (1 - b1) * in[1] -> gl_Position[3]);
                    d2[0].data[i] = a1 * in[0] -> data[i] + (1 - a1) * in[1] -> data[i];
                    break;
                    
                default:
                    break;
            }
            
        }
        
        d2[0].gl_Position = p1;
        in3[0] = &d2[0];
        in3[1] = &d1[1];
        in3[2] = &d1[0];
        
    }
    
    clip_triangle(state, in3, face + 1);
}

// Rasterize the triangle defined by the three vertices in the "in" array.  This
// function is responsible for rasterization, interpolation of data to
// fragments, calling the fragment shader, and z-buffering.
void rasterize_triangle(driver_state& state, const data_geometry* in[3])
{
    //std::cout<<"TODO: implement rasterization"<<std::endl;
    //i = w/2 * x + w/2 - 1/2
    //j = h/2 * y + h/2 - 1/2
    float x[3], y[3];
    float temp_x, temp_y;
    //Calculats i & j coordinates in NDC for vertices
    
    for(unsigned int a = 0; a < 3; ++a){
        temp_x = (state.image_width / 2.0)
                                 * (in[a]->gl_Position[0]/in[a]->gl_Position[3])
                                 + (state.image_width / 2.0) - (0.5);
        temp_y = (state.image_height / 2.0)
                                 * (in[a]->gl_Position[1]/in[a]->gl_Position[3])
                                 + (state.image_height / 2.0) - (0.5);
        x[a] = temp_x;
        y[a] = temp_y;
        //std::cout <<"temp_x value: " <<temp_x <<"\n";
        //std::cout <<"temp_y value: " <<temp_y <<"\n";

        
        temp_x = 0.0;
        temp_y = 0.0;
        //state.image_color[i + j * state.image_width] = make_pixel(255, 255, 255);
        
    }


    
    //finds the min/max of triangle
    float min_x = std::min(x[0], std::min(x[1], x[2]));
    float max_x = std::max(x[0], std::max(x[1], x[2]));
    float min_y = std::min(y[0], std::min(y[1], y[2]));
    float max_y = std::max(y[0], std::max(y[1], y[2]));
    
    
    //Makes sure triangle is within pixel grid
    if(min_x < 0){
        min_x = 0;
    }
    if(min_y < 0){
        min_y = 0;
    }
    if(max_x > state.image_width){
        max_x = state.image_width;
    }
    if(max_y > state.image_height){
        max_y = state.image_height;
    }
    
    //calculates area of the triangle
    float area_abc = (0.5 * ((x[1]*y[2] - x[2]*y[1]) - (x[0]*y[2] - x[2]*y[0]) + (x[0]*y[1] - x[1]*y[0])));
    
    //float *data = new float[MAX_FLOATS_PER_VERTEX];
    data_fragment fragment_data;
    fragment_data.data = new float[MAX_FLOATS_PER_VERTEX];
    
    data_output output;
    
    for(int j = min_y; j < max_y; ++j){
        for(int i = min_x; i < max_x; ++i){
            
            float alpha = (0.5 * ((x[1] * y[2] - x[2] * y[1]) + (y[1] - y[2])*i + (x[2] - x[1])*j)) / area_abc;
            float beta = (0.5 * ((x[2] * y[0] - x[0] * y[2]) + (y[2] - y[0])*i + (x[0] - x[2])*j)) / area_abc;
            float gamma = (0.5 * ((x[0] * y[1] - x[1] * y[0]) + (y[0] - y[1])*i + (x[1] - x[0])*j)) / area_abc;
            
            float depth = (alpha * in[0]->gl_Position[2]/in[0]->gl_Position[3]) + (beta * in[1]->gl_Position[2]/in[1]->gl_Position[3]) + (gamma * in[2]->gl_Position[2]/in[2]->gl_Position[3]);
            
            
            if(alpha >= 0 && beta >= 0 && gamma >= 0 && depth < state.image_depth[i + j * state.image_width]){
                const float alpha_p = alpha;
                const float beta_p = beta;
                const float gamma_p = gamma;
                
                for(int k = 0; k < state.floats_per_vertex; ++k){
                    float k_gour;
                    
                    switch(state.interp_rules[k]){
                        case interp_type::flat:
                            
                            fragment_data.data[k] = in[0]->data[k];
                            
                            break;
                        case interp_type::smooth:
                            
                            k_gour = (alpha_p / in[0]->gl_Position[3])
                            + (beta_p / in[1]->gl_Position[3])
                            + (gamma_p / in[2]->gl_Position[3]);
                            
                            
                            alpha = alpha_p / k_gour / (in[0]->gl_Position[3]);
                            beta = beta_p / k_gour / (in[1]->gl_Position[3]);
                            gamma = gamma_p / k_gour / (in[2]->gl_Position[3]);
                            
                        case interp_type::noperspective:
                            
                            fragment_data.data[k] = alpha * in[0]->data[k]
                            + beta * in[1]->data[k]
                            + gamma * in[2]->data[k];
                            
                            break;
                        default:
                            break;
                            
                            
                    }
                }
                
                
                
                state.fragment_shader(fragment_data, output, state.uniform_data);
                //out_data.output_color = out_data.output_color * 255;
                
                state.image_color[i + j * state.image_width] = make_pixel((output.output_color[0] * 255),
                                                                          (output.output_color[1] * 255),
                                                                          (output.output_color[2] * 255));
                state.image_depth[i + j * state.image_width] = depth;
            }
        }
    }
    //std::cout<<"TODO: implement rasterization"<<std::endl;
 //   delete [] data;
}


