#pragma once

class Quad
{
public:
    explicit Quad() {}
    Quad(const Quad &) = delete;
    Quad(Quad &&) = delete;
    Quad& operator=(const Quad &) = delete;
    Quad& operator=(Quad &&) = delete;
    ~Quad();

    void Create();
    void Draw() const;

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
};
