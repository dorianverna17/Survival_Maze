#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "components/simple_scene.h"
#include "lab_m1/Tema2/Tema2.h"

namespace helpers
{
    inline Mesh* CreateSquare(
        const std::string& name,
        glm::vec3 leftBottomCorner,
        float length,
        glm::vec3 color,
        bool fill)
    {
        glm::vec3 corner = leftBottomCorner;

        std::vector<VertexFormat> vertices =
        {
            VertexFormat(corner, color),
            VertexFormat(corner + glm::vec3(length, 0, 0), color),
            VertexFormat(corner + glm::vec3(length, length, 0), color),
            VertexFormat(corner + glm::vec3(0, length, 0), color)
        };

        Mesh* square = new Mesh(name);
        std::vector<unsigned int> indices = { 0, 1, 2, 3 };

        if (!fill) {
            square->SetDrawMode(GL_LINE_LOOP);
        }
        else {
            indices.push_back(0);
            indices.push_back(2);
        }

        square->InitFromData(vertices, indices);
        return square;
    }

    inline glm::mat3 Translate2D(float translateX, float translateY)
    {
        return glm::transpose(
            glm::mat3(1, 0, translateX,
                0, 1, translateY,
                0, 0, 1)
        );
    }

    inline glm::mat3 Scale2D(float scaleX, float scaleY)
    {
        return glm::transpose(
            glm::mat3(scaleX, 0, 0,
                0, scaleY, 0,
                0, 0, 1)
        );

    }

    inline glm::mat3 Rotate2D(float radians)
    {
        return glm::transpose(
            glm::mat3(cos(radians), -sin(radians), 0,
                sin(radians), cos(radians), 0,
                0, 0, 1)
        );

    }

    inline glm::mat4 Translate(float translateX, float translateY, float translateZ)
    {
        return glm::transpose(
            glm::mat4(1, 0, 0, translateX,
                0, 1, 0, translateY,
                0, 0, 1, translateZ,
                0, 0, 0, 1)
        );
    }

    inline glm::mat4 Scale(float scaleX, float scaleY, float scaleZ)
    {
        return glm::transpose(
            glm::mat4(scaleX, 0, 0, 0,
                0, scaleY, 0, 0,
                0, 0, scaleZ, 0,
                0, 0, 0, 1)
        );
    }

    inline glm::mat4 RotateOZ(float radians)
    {
        return glm::transpose(
            glm::mat4(cos(radians), -sin(radians), 0, 0,
                sin(radians), cos(radians), 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1)
        );

    }

    inline glm::mat4 RotateOY(float radians)
    {
        return glm::transpose(
            glm::mat4(cos(radians), 0, sin(radians), 0,
                0, 1, 0, 0,
                -sin(radians), 0, cos(radians), 0,
                0, 0, 0, 1)
        );
    }

    inline glm::mat4 RotateOX(float radians)
    {
        return glm::transpose(
            glm::mat4(1, 0, 0, 0,
                0, cos(radians), -sin(radians), 0,
                0, sin(radians), cos(radians), 0,
                0, 0, 0, 1)
        );
    }

    class Camera
    {
    public:
        Camera()
        {
            position = glm::vec3(0, 2, 5);
            forward = glm::vec3(0, 0, -1);
            up = glm::vec3(0, 1, 0);
            right = glm::vec3(1, 0, 0);
            distanceToTarget = 2;
        }

        Camera(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
        {
            Set(position, center, up);
        }

        ~Camera()
        { }

        void Set(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
        {
            this->position = position;
            forward = glm::normalize(center - position);
            right = glm::cross(forward, up);
            this->up = glm::cross(right, forward);
        }

        void MoveForward(float distance)
        {
            glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
            position += dir * distance;
        }

        void TranslateForward(float distance)
        {
            position += glm::normalize(forward) * distance;
        }

        void TranslateUpward(float distance)
        {
            position += glm::normalize(up) * distance;
        }

        void TranslateRight(float distance)
        {
            position += glm::normalize(right) * distance;
        }

        void RotateFirstPerson_OX(float angle)
        {
            forward = glm::normalize(glm::rotate(glm::mat4(1), angle, right) * glm::vec4(forward, 1));
            up = glm::normalize(glm::cross(right, forward));
        }

        void RotateFirstPerson_OY(float angle)
        {
            forward = glm::normalize(glm::rotate(glm::mat4(1.f), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 0));
            right = glm::normalize(glm::rotate(glm::mat4(1.f), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1));
            up = glm::cross(right, forward);
        }

        void RotateFirstPerson_OZ(float angle)
        {
            glm::vec4 aux = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1);
            right = glm::normalize(glm::vec3(aux));
            aux = glm::rotate(glm::mat4(1.f), angle, forward) * glm::vec4(up, 0);
            forward = glm::normalize(glm::vec3(aux));
            up = glm::cross(right, forward);
        }

        void RotateThirdPerson_OX(float angle)
        {
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OX(angle);
            TranslateForward(-distanceToTarget);
        }

        void RotateThirdPerson_OY(float angle)
        {
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OY(angle);
            TranslateForward(-distanceToTarget);
        }

        void RotateThirdPerson_OZ(float angle)
        {
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OZ(angle);
            TranslateForward(-distanceToTarget);
        }

        glm::mat4 GetViewMatrix()
        {
            return glm::lookAt(position, position + forward, up);
        }

        glm::vec3 GetTargetPosition()
        {
            return position + forward * distanceToTarget;
        }

    public:
        float distanceToTarget;
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
    };
} // namespace helpers
