#pragma once

class Quad
{
public:
    explicit Quad() {}
    ~Quad();

    void Create();
    void Draw() const;

private:
    unsigned int VAO = -1;
    unsigned int VBO = -1;
};
