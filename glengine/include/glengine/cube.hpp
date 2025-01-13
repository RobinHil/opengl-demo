#ifndef GLENGINE_CUBE_HPP
#define GLENGINE_CUBE_HPP

namespace GLEngine {
    class Cube {
    public:
        Cube(float size = 0.1f);
        ~Cube();
        
        void draw() const;
        void cleanup();
        
    private:
        unsigned int VAO, VBO;
        
        void setupCube(float size);
    };
}

#endif // GLENGINE_CUBE_HPP