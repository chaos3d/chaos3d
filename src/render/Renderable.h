// ----------------------------------------------
//
// Renderable.h
//
// Created by wuh (2/23)
// Copyright (c) 2009
//
// ----------------------------------------------


#ifndef	_CHAOS_RENDERABLE_H
#define	_CHAOS_RENDERABLE_H

#include "chaos_config.h"

_CHAOS_BEGIN

class Material;
struct VertexData;

/*
A renderable is a whole pipeline of rendering operation, including
loading shaders, setting up the render state, assigning variables for
shaders, loading textures, and issuing vertices.

all of above but last one is composed of Material, and the last one,
probably with some assignments, such as matrics, shaders' variables
as ones that are collected as like vertex declarations.

Usually, it is not directly created manually but by tools such as max
or maya exporters, or by scene manager like 2d batching system. because
it is too complicated to maintain.

For now, I think it is generally created by some loaders from files.
For example, a model with some meshes, some bones and materials, is
loaded by the resource manager, then several renderables, like submash
with variables' settings are created and aggregated into, probably 
scene node, called Entity, which could be a component managed by some
scene manager. That said, resouce manager uses the factory to create
mesh or bones and then some game object with a certain component that
created a renderable with referred meshes and bones.

//NOPE
NB: Renderable is like render operation but with more info. That said
renderable is usually created as needed and push into render queue.
And thus, the scene manager is usually doing this. 

every scene node may contain some information of how it is to be ren-
dered, and its vertices, which are pulled out to renderables.

Tha is the renderable that will be sorted by render system in order to
speeding up.
*/
struct Renderable{
	Material*		material;
	VertexData*		vertexData;

	Renderable() : material(0), vertexData(0) {};
};

_CHAOS_END

#endif