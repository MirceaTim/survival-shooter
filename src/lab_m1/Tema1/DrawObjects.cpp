#include "DrawObjects.h"

#include <vector>
#include <math.h>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* DrawObjects::CreateRectangle(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    float width,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, width, 0), color),
        VertexFormat(corner + glm::vec3(0, width, 0), color)
    };

    Mesh* rectangle = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        rectangle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices.push_back(0);
        indices.push_back(2);
    }

    rectangle->InitFromData(vertices, indices);
    return rectangle;
}
Mesh* DrawObjects::CreateCircle(const std::string& name, glm::vec3 centre, float radius, glm::vec3 color) {
    int i;
    int triangleAmount = 100;

    std::vector<VertexFormat> vertices1 = {
        VertexFormat(centre, color)
    };
    std::vector<unsigned int> indices1 = {};
    Mesh* circle = new Mesh(name);

    for (i = 0; i <= triangleAmount; i++) {
        vertices1.push_back(
            VertexFormat(glm::vec3(centre.x + (radius * cos(2 * i * M_PI / triangleAmount)),
                centre.y + (radius * sin(2 * i * M_PI / triangleAmount)), 0), color));
        indices1.push_back(0);
        indices1.push_back(i + 1);
        indices1.push_back(i + 2);
    }
    circle->InitFromData(vertices1, indices1);
    return circle;
}
