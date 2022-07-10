#pragma once

#include <fcpw/utilities/scene_data.h>

namespace fcpw {

enum class PrimitiveType {
	LineSegment,
	Triangle
};

template<size_t DIM>
class Scene {
public:
	// constructor
	Scene();

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// API to specify scene geometry

	// sets the type of primitives the objects in the scene contain;
	// e.g., {{LineSegment}, {Triangle}, {LineSegment, Triangle}} specifies a scene
	// with 3 objects, the 1st containing line segments, the 2nd containing triangles
	// and the 3rd containing a mix of line segments and triangles; each call
	// to this function resets the scene data
	void setObjectTypes(const std::vector<std::vector<PrimitiveType>>& objectTypes);

	// sets the number of vertices in an object
	void setObjectVertexCount(int nVertices, int objectIndex);

	// sets the number of line segments in an object
	void setObjectLineSegmentCount(int nLineSegments, int objectIndex);

	// sets the number of triangles in an object
	void setObjectTriangleCount(int nTriangles, int objectIndex);

	// sets the position of a vertex in an object
	void setObjectVertex(const Vector<DIM>& position, int vertexIndex, int objectIndex);

	// sets the vertex indices of a line segment in an object
	void setObjectLineSegment(const int *indices, int lineSegmentIndex, int objectIndex);

	// sets the vertex indices of a triangle in an object
	void setObjectTriangle(const int *indices, int triangleIndex, int objectIndex);

	// sets the vertex indices of a primitive in an object; primitiveIndex must lie in the range
	// [0, nLineSegments) or [0, nTriangles) based on the primitive type; internally, the line
	// segments are stored before the triangles; NOTE: use this function only if an object contains
	// mixed primitive types, otherwise use one of the two functions above
	void setObjectPrimitive(const int *indices, const PrimitiveType& primitiveType,
							int primitiveIndex, int objectIndex);

	// sets the instance transforms for an object
	void setObjectInstanceTransforms(const std::vector<Transform<DIM>>& transforms, int objectIndex);

	// sets the data for a node in the csg tree; NOTE: the root node of the csg tree must have index 0
	void setCsgTreeNode(const CsgTreeNode& csgTreeNode, int nodeIndex);

	// enables normal computation for an object with a single primitive type; if normals are
	// required for an object with mixed primitive types, they can be computed after performing
	// a query using the "primitiveIndex" member in the "Interaction" class. NOTE: enabling normal
	// computation for non-planar line segments produces gargabe results since they are not well defined
	void computeObjectNormals(int objectIndex);

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// API to build the scene aggregate/accelerator

	// builds a (possibly vectorized) aggregate/accelerator for the scene; each call to this
	// function rebuilds the aggregate/accelerator for the scene from the specified geometry
	// (except when reduceMemoryFootprint is set to true which results in undefined behavior);
	// it is recommended to set vectorize to false for primitives that do not implement
	// vectorized intersection and closest point queries; set reduceMemoryFootprint to true
	// to reduce the memory footprint of fcpw when constructing an aggregate, however if you
	// plan to access the scene data let it remain false
	void build(const AggregateType& aggregateType, bool vectorize,
			   bool printStats=false, bool reduceMemoryFootprint=false);

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// API to find closest points and intersect rays with the scene, among others

	// intersects the scene with the given ray and returns the number of hits;
	// by default, returns the closest interaction if it exists;
	// if checkForOcclusion is enabled, the interactions vector is not populated;
	// if recordAllHits is enabled, sorts the interactions by distance to the ray origin
	int intersect(Ray<DIM>& r, std::vector<Interaction<DIM>>& is,
				  bool checkForOcclusion=false, bool recordAllHits=false) const;

	// checks whether a point is contained inside a scene; NOTE: the scene must be watertight
	bool contains(const Vector<DIM>& x) const;

	// checks whether there is a line of sight between between two point
	bool hasLineOfSight(const Vector<DIM>& xi, const Vector<DIM>& xj) const;

	// finds the closest point in the scene to a point; optionally specify a conservative
	// radius guess around the point within which the closest point can be found
	bool findClosestPoint(const Vector<DIM>& x, Interaction<DIM>& i,
						  float squaredRadius=maxFloat) const;

	// returns a pointer to the underlying scene data; use at your own risk...
	SceneData<DIM>* getSceneData();

private:
	// member
	std::unique_ptr<SceneData<DIM>> sceneData;
};

} // namespace fcpw

#include "fcpw.inl"
