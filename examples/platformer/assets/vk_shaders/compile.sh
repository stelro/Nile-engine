#!/usr/bin/env bash
../../bin/glslc  shader.vert -o shader.frag.spv
../../bin/glslc  shader.frag -o shader.vert.spv
