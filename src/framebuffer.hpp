#include <iostream>
#include <glad/gl.h>
#include <shader.h>
#include <string.h>

class FrameBuffer {
  public:
    int screen_width;
    int screen_height;
    Shader screenShader;
    unsigned int framebuffer;
    unsigned int textureColorbuffer;
    unsigned int quadVAO, quadVBO;
    unsigned int rbo;

    FrameBuffer(Shader &shader, int s_w, int s_h): screenShader(shader) {
      screenShader = shader;
      screen_width = s_w;
      screen_height = s_h;
    }

    void setupFullScreenQuad() {
      float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
          // positions   // texCoords
          -1.0f,  1.0f,  0.0f, 1.0f,
          -1.0f, -1.0f,  0.0f, 0.0f,
           1.0f, -1.0f,  1.0f, 0.0f,
          -1.0f,  1.0f,  0.0f, 1.0f,
           1.0f, -1.0f,  1.0f, 0.0f,
           1.0f,  1.0f,  1.0f, 1.0f
      };

      glGenVertexArrays(1, &quadVAO);
      glGenBuffers(1, &quadVBO);
      glBindVertexArray(quadVAO);
      glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    void checkFrameBuffer() {
      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
          std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void generateColorTexture() {
      glGenTextures(1, &textureColorbuffer);
      glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    }

    void generateRenderBuffer() {
      glGenRenderbuffers(1, &rbo);
      glBindRenderbuffer(GL_RENDERBUFFER, rbo);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen_width, screen_height);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    }

    void setup() {
      setupFullScreenQuad();
      glGenFramebuffers(1, &framebuffer);
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
      generateColorTexture();
      generateRenderBuffer();
      checkFrameBuffer();
    }

    void clearAndRenderQuad() {
      screenShader.use();
      screenShader.setInt("screenTexture", 0);
      glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
      glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
      glClear(GL_COLOR_BUFFER_BIT);
      glBindVertexArray(quadVAO);
      glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void renderToFramebuffer() {
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
    }
};
