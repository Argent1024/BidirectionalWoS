#include <fcpw/core/wide_query_operations.h>

namespace fcpw {

template<size_t WIDTH, size_t DIM, typename PrimitiveType>
inline int Mbvh<WIDTH, DIM, PrimitiveType>::collapseSbvh(const Sbvh<DIM, PrimitiveType> *sbvh,
														 int sbvhNodeIndex, int parent, int depth)
{
	const SbvhNode<DIM>& sbvhNode = sbvh->flatTree[sbvhNodeIndex];
	maxDepth = std::max(depth, maxDepth);

	// create mbvh node
	MbvhNode<DIM> mbvhNode;
	int mbvhNodeIndex = nNodes;

	nNodes++;
	flatTree.emplace_back(mbvhNode);

	if (sbvhNode.nReferences > 0) {
		// sbvh node is a leaf node; assign mbvh node its reference indices
		MbvhNode<DIM>& mbvhNode = flatTree[mbvhNodeIndex];
		mbvhNode.child[0] = -(nLeafs + 1); // negative value indicates that node is a leaf
		mbvhNode.child[1] = sbvhNode.nReferences/WIDTH;
		if (sbvhNode.nReferences%WIDTH != 0) mbvhNode.child[1] += 1;
		mbvhNode.child[2] = sbvhNode.referenceOffset;
		mbvhNode.child[3] = sbvhNode.nReferences;
		nLeafs += mbvhNode.child[1];

	} else {
		// sbvh node is an inner node, flatten it
		int nNodesToCollapse = 2;
		int nodesToCollapse[FCPW_MBVH_BRANCHING_FACTOR];
		nodesToCollapse[0] = sbvhNodeIndex + sbvhNode.secondChildOffset;
		nodesToCollapse[1] = sbvhNodeIndex + 1;
		bool noMoreNodesToCollapse = false;

		while (nNodesToCollapse < FCPW_MBVH_BRANCHING_FACTOR && !noMoreNodesToCollapse) {
			// find the (non-leaf) node entry with the largest surface area
			float maxSurfaceArea = minFloat;
			int maxIndex = -1;

			for (int i = 0; i < nNodesToCollapse; i++) {
				int sbvhNodeIndex = nodesToCollapse[i];
				const SbvhNode<DIM>& sbvhNode = sbvh->flatTree[sbvhNodeIndex];

				if (sbvhNode.nReferences == 0) {
					float surfaceArea = sbvhNode.box.surfaceArea();

					if (maxSurfaceArea < surfaceArea) {
						maxSurfaceArea = surfaceArea;
						maxIndex = i;
					}
				}
			}

			if (maxIndex == -1) {
				// no more nodes to collapse
				noMoreNodesToCollapse = true;

			} else {
				// remove the selected node from the list, and add its two children
				int sbvhNodeIndex = nodesToCollapse[maxIndex];
				const SbvhNode<DIM>& sbvhNode = sbvh->flatTree[sbvhNodeIndex];

				nodesToCollapse[maxIndex] = sbvhNodeIndex + sbvhNode.secondChildOffset;
				nodesToCollapse[nNodesToCollapse] = sbvhNodeIndex + 1;
				nNodesToCollapse++;
			}
		}

		// collapse the nodes
		std::sort(nodesToCollapse, nodesToCollapse + nNodesToCollapse);
		for (int i = 0; i < nNodesToCollapse; i++) {
			int sbvhNodeIndex = nodesToCollapse[i];
			const SbvhNode<DIM>& sbvhNode = sbvh->flatTree[sbvhNodeIndex];

			// assign mbvh node this sbvh node's bounding box and index
			for (size_t j = 0; j < DIM; j++) {
				flatTree[mbvhNodeIndex].boxMin[j][i] = sbvhNode.box.pMin[j];
				flatTree[mbvhNodeIndex].boxMax[j][i] = sbvhNode.box.pMax[j];
			}

			flatTree[mbvhNodeIndex].child[i] = collapseSbvh(sbvh, sbvhNodeIndex, mbvhNodeIndex, depth + 1);
		}
	}

	return mbvhNodeIndex;
}

template<size_t WIDTH, size_t DIM, typename PrimitiveType>
inline bool Mbvh<WIDTH, DIM, PrimitiveType>::isLeafNode(const MbvhNode<DIM>& node) const
{
	return node.child[0] < 0;
}

template<size_t WIDTH, size_t DIM, typename PrimitiveType>
inline void populateLeafNode(const MbvhNode<DIM>& node, const std::vector<PrimitiveType *>& primitives,
							 std::vector<MbvhLeafNode<WIDTH, DIM, PrimitiveType>>& leafNodes)
{
	std::cerr << "populateLeafNode(): WIDTH: " << WIDTH << ", DIM: " << DIM << " not supported" << std::endl;
	exit(EXIT_FAILURE);
}

template<size_t WIDTH>
inline void populateLeafNode(const MbvhNode<3>& node, const std::vector<LineSegment *>& primitives,
							 std::vector<MbvhLeafNode<WIDTH, 3, LineSegment>>& leafNodes)
{
	int leafOffset = -node.child[0] - 1;
	int referenceOffset = node.child[2];
	int nReferences = node.child[3];

	// populate leaf node with line segments
	for (int p = 0; p < nReferences; p++) {
		int referenceIndex = referenceOffset + p;
		int leafIndex = leafOffset + p/WIDTH;
		int w = p%WIDTH;

		const LineSegment *lineSegment = primitives[referenceIndex];
		const Vector3& pa = lineSegment->soup->positions[lineSegment->indices[0]];
		const Vector3& pb = lineSegment->soup->positions[lineSegment->indices[1]];

		leafNodes[leafIndex].primitiveIndex[w] = lineSegment->pIndex;
		for (int i = 0; i < 3; i++) {
			leafNodes[leafIndex].positions[0][i][w] = pa[i];
			leafNodes[leafIndex].positions[1][i][w] = pb[i];
		}
	}
}

template<size_t WIDTH>
inline void populateLeafNode(const MbvhNode<3>& node, const std::vector<Triangle *>& primitives,
							 std::vector<MbvhLeafNode<WIDTH, 3, Triangle>>& leafNodes)
{
	int leafOffset = -node.child[0] - 1;
	int referenceOffset = node.child[2];
	int nReferences = node.child[3];

	// populate leaf node with triangles
	for (int p = 0; p < nReferences; p++) {
		int referenceIndex = referenceOffset + p;
		int leafIndex = leafOffset + p/WIDTH;
		int w = p%WIDTH;

		const Triangle *triangle = primitives[referenceIndex];
		const Vector3& pa = triangle->soup->positions[triangle->indices[0]];
		const Vector3& pb = triangle->soup->positions[triangle->indices[1]];
		const Vector3& pc = triangle->soup->positions[triangle->indices[2]];

		leafNodes[leafIndex].primitiveIndex[w] = triangle->pIndex;
		for (int i = 0; i < 3; i++) {
			leafNodes[leafIndex].positions[0][i][w] = pa[i];
			leafNodes[leafIndex].positions[1][i][w] = pb[i];
			leafNodes[leafIndex].positions[2][i][w] = pc[i];
		}
	}
}

template<size_t WIDTH, size_t DIM, typename PrimitiveType>
inline void Mbvh<WIDTH, DIM, PrimitiveType>::populateLeafNodes()
{
	if (vectorizedLeafType == ObjectType::LineSegments ||
		vectorizedLeafType == ObjectType::Triangles) {
		leafNodes.resize(nLeafs);

		for (int i = 0; i < nNodes; i++) {
			MbvhNode<DIM>& node = flatTree[i];
			if (isLeafNode(node)) populateLeafNode(node, primitives, leafNodes);
		}
	}
}

template<size_t WIDTH, size_t DIM, typename PrimitiveType>
inline Mbvh<WIDTH, DIM, PrimitiveType>::Mbvh(const Sbvh<DIM, PrimitiveType> *sbvh_, bool printStats_):
nNodes(0),
nLeafs(0),
maxDepth(0),
area(0.0f),
volume(0.0f),
primitives(sbvh_->primitives),
primitiveTypeIsAggregate(std::is_base_of<Aggregate<DIM>, PrimitiveType>::value),
range(enoki::arange<enoki::Array<int, DIM>>())
{
	static_assert(FCPW_MBVH_BRANCHING_FACTOR == 4 || FCPW_MBVH_BRANCHING_FACTOR == 8,
				  "Branching factor must be atleast 4");

	using namespace std::chrono;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	// collapse sbvh
	collapseSbvh(sbvh_, 0, 0xfffffffc, 0);

	// determine object type
	vectorizedLeafType = std::is_same<PrimitiveType, LineSegment>::value ? ObjectType::LineSegments :
						 std::is_same<PrimitiveType, Triangle>::value ? ObjectType::Triangles :
						 ObjectType::Mixed;

	// populate leaf nodes if primitive type is supported
	populateLeafNodes();

	// precompute surface area and signed volume
	int nPrimitives = (int)primitives.size();
	aggregateCentroid = Vector<DIM>::Zero();

	for (int p = 0; p < nPrimitives; p++) {
		aggregateCentroid += primitives[p]->centroid();
		area += primitives[p]->surfaceArea();
		volume += primitives[p]->signedVolume();
	}

	aggregateCentroid /= nPrimitives;

	// don't compute normals by default
	this->computeNormals = false;

	// print stats
	if (printStats_) {
		// count not-full nodes
		float nLeafsNotFull = 0.0f;
		float nNodesNotFull = 0.0f;
		int nInnerNodes = 0;

		for (int i = 0; i < nNodes; i++) {
			MbvhNode<DIM>& node = flatTree[i];

			if (isLeafNode(node)) {
				if (node.child[3]%WIDTH != 0) {
					nLeafsNotFull += 1.0f;
				}

			} else {
				nInnerNodes++;
				for (int w = 0; w < FCPW_MBVH_BRANCHING_FACTOR; w++) {
					if (node.child[w] == maxInt) {
						nNodesNotFull += 1.0f;
						break;
					}
				}
			}
		}

		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		duration<double> timeSpan = duration_cast<duration<double>>(t2 - t1);
		std::cout << "Built " << FCPW_MBVH_BRANCHING_FACTOR << "-bvh with "
				  << nNodes << " nodes, "
				  << nLeafs << " leaves, "
				  << (nNodesNotFull*100/nInnerNodes) << "% nodes & "
				  << (nLeafsNotFull*100/nLeafs) << "% leaves not full, "
				  << maxDepth << " max depth, "
				  << primitives.size() << " primitives in "
				  << timeSpan.count() << " seconds" << std::endl;
	}
}

template<size_t WIDTH, size_t DIM, typename PrimitiveType>
inline BoundingBox<DIM> Mbvh<WIDTH, DIM, PrimitiveType>::boundingBox() const
{
	BoundingBox<DIM> box;
	if (flatTree.size() == 0) return box;

	enoki::scatter(box.pMin.data(), enoki::hmin_inner(flatTree[0].boxMin), range);
	enoki::scatter(box.pMax.data(), enoki::hmax_inner(flatTree[0].boxMax), range);

	return box;
}

template<size_t WIDTH, size_t DIM, typename PrimitiveType>
inline Vector<DIM> Mbvh<WIDTH, DIM, PrimitiveType>::centroid() const
{
	return aggregateCentroid;
}

template<size_t WIDTH, size_t DIM, typename PrimitiveType>
inline float Mbvh<WIDTH, DIM, PrimitiveType>::surfaceArea() const
{
	return area;
}

template<size_t WIDTH, size_t DIM, typename PrimitiveType>
inline float Mbvh<WIDTH, DIM, PrimitiveType>::signedVolume() const
{
	return volume;
}

template<size_t WIDTH, size_t DIM>
inline void enqueueNodes(const MbvhNode<DIM>& node, const FloatP<WIDTH>& tMin,
						 const FloatP<WIDTH>& tMax, const MaskP<WIDTH>& mask,
						 float minDist, float& tMaxMin, int& stackPtr, BvhTraversal *subtree)
{
	// enqueue nodes
	int closestIndex = -1;
	for (int w = 0; w < WIDTH; w++) {
		if (mask[w]) {
			stackPtr++;
			subtree[stackPtr].node = node.child[w];
			subtree[stackPtr].distance = tMin[w];
			tMaxMin = std::min(tMaxMin, tMax[w]);

			if (tMin[w] < minDist) {
				closestIndex = stackPtr;
				minDist = tMin[w];
			}
		}
	}

	// put closest node first
	if (closestIndex != -1) {
		std::swap(subtree[stackPtr], subtree[closestIndex]);
	}
}

inline void sortOrder4(const FloatP<4>& t, int& a, int& b, int& c, int& d)
{
	// source: https://stackoverflow.com/questions/25070577/sort-4-numbers-without-array
	int tmp;
	if (t[a] < t[b]) { tmp = a; a = b; b = tmp; }
	if (t[c] < t[d]) { tmp = c; c = d; d = tmp; }
	if (t[a] < t[c]) { tmp = a; a = c; c = tmp; }
	if (t[b] < t[d]) { tmp = b; b = d; d = tmp; }
	if (t[b] < t[c]) { tmp = b; b = c; c = tmp; }
}

template<size_t DIM>
inline void enqueueNodes(const MbvhNode<DIM>& node, const FloatP<4>& tMin,
						 const FloatP<4>& tMax, const MaskP<4>& mask,
						 float minDist, float& tMaxMin, int& stackPtr, BvhTraversal *subtree)
{
	// sort nodes
	int order[4] = {0, 1, 2, 3};
	sortOrder4(tMin, order[0], order[1], order[2], order[3]);

	// enqueue overlapping nodes in sorted order
	for (int w = 0; w < 4; w++) {
		int W = order[w];

		if (mask[W]) {
			stackPtr++;
			subtree[stackPtr].node = node.child[W];
			subtree[stackPtr].distance = tMin[W];
			tMaxMin = std::min(tMaxMin, tMax[W]);
		}
	}
}

template<size_t WIDTH, size_t DIM, typename PrimitiveType>
inline int intersectPrimitives(const MbvhNode<DIM>& node,
							   const std::vector<MbvhLeafNode<WIDTH, DIM, PrimitiveType>>& leafNodes,
							   int nodeIndex, int aggregateIndex, const enokiVector<DIM>& ro, const enokiVector<DIM>& rd,
							   float& rtMax, std::vector<Interaction<DIM>>& is, bool recordAllHits)
{
	std::cerr << "intersectPrimitives(): WIDTH: " << WIDTH << ", DIM: " << DIM << " not supported" << std::endl;
	exit(EXIT_FAILURE);

	return 0;
}

template<size_t WIDTH>
inline int intersectPrimitives(const MbvhNode<3>& node,
							   const std::vector<MbvhLeafNode<WIDTH, 3, LineSegment>>& leafNodes,
							   int nodeIndex, int aggregateIndex, const enokiVector3& ro, const enokiVector3& rd,
							   float& rtMax, std::vector<Interaction<3>>& is, bool recordAllHits)
{
	int leafOffset = -node.child[0] - 1;
	int nLeafs = node.child[1];
	int referenceOffset = node.child[2];
	int nReferences = node.child[3];
	int startReference = 0;
	int hits = 0;

	for (int l = 0; l < nLeafs; l++) {
		// perform vectorized intersection query
		FloatP<WIDTH> d;
		Vector3P<WIDTH> pt;
		FloatP<WIDTH> t;
		int leafIndex = leafOffset + l;
		const Vector3P<WIDTH>& pa = leafNodes[leafIndex].positions[0];
		const Vector3P<WIDTH>& pb = leafNodes[leafIndex].positions[1];
		const IntP<WIDTH>& primitiveIndex = leafNodes[leafIndex].primitiveIndex;
		MaskP<WIDTH> mask = intersectWideLineSegment<WIDTH>(pa, pb, ro, rd, rtMax, d, pt, t);

		if (recordAllHits) {
			// record interactions
			int endReference = startReference + WIDTH;
			if (endReference > nReferences) endReference = nReferences;

			for (int p = startReference; p < endReference; p++) {
				int w = p - startReference;

				if (mask[w]) {
					hits++;
					auto it = is.emplace(is.end(), Interaction<3>());
					it->d = d[w];
					it->p[0] = pt[0][w];
					it->p[1] = pt[1][w];
					it->p[2] = pt[2][w];
					it->uv[0] = t[w];
					it->uv[1] = -1;
					it->primitiveIndex = primitiveIndex[w];
					it->nodeIndex = nodeIndex;
					it->referenceIndex = referenceOffset + p;
					it->objectIndex = aggregateIndex;
				}
			}

		} else {
			// determine closest index
			int closestIndex = -1;
			int W = std::min((int)WIDTH, nReferences - startReference);

			for (int w = 0; w < W; w++) {
				if (mask[w] && d[w] <= rtMax) {
					closestIndex = w;
					rtMax = d[w];
				}
			}

			// update interaction
			if (closestIndex != -1) {
				hits = 1;
				is[0].d = d[closestIndex];
				is[0].p[0] = pt[0][closestIndex];
				is[0].p[1] = pt[1][closestIndex];
				is[0].p[2] = pt[2][closestIndex];
				is[0].uv[0] = t[closestIndex];
				is[0].uv[1] = -1;
				is[0].primitiveIndex = primitiveIndex[closestIndex];
				is[0].nodeIndex = nodeIndex;
				is[0].referenceIndex = referenceOffset + startReference + closestIndex;
				is[0].objectIndex = aggregateIndex;
			}
		}

		startReference += WIDTH;
	}

	return hits;
}

template<size_t WIDTH>
inline int intersectPrimitives(const MbvhNode<3>& node,
							   const std::vector<MbvhLeafNode<WIDTH, 3, Triangle>>& leafNodes,
							   int nodeIndex, int aggregateIndex, const enokiVector3& ro, const enokiVector3& rd,
							   float& rtMax, std::vector<Interaction<3>>& is, bool recordAllHits)
{
	int leafOffset = -node.child[0] - 1;
	int nLeafs = node.child[1];
	int referenceOffset = node.child[2];
	int nReferences = node.child[3];
	int startReference = 0;
	int hits = 0;

	for (int l = 0; l < nLeafs; l++) {
		// perform vectorized intersection query
		FloatP<WIDTH> d;
		Vector3P<WIDTH> pt;
		Vector2P<WIDTH> t;
		int leafIndex = leafOffset + l;
		const Vector3P<WIDTH>& pa = leafNodes[leafIndex].positions[0];
		const Vector3P<WIDTH>& pb = leafNodes[leafIndex].positions[1];
		const Vector3P<WIDTH>& pc = leafNodes[leafIndex].positions[2];
		const IntP<WIDTH>& primitiveIndex = leafNodes[leafIndex].primitiveIndex;
		MaskP<WIDTH> mask = intersectWideTriangle<WIDTH>(pa, pb, pc, ro, rd, rtMax, d, pt, t);

		if (recordAllHits) {
			// record interactions
			int endReference = startReference + WIDTH;
			if (endReference > nReferences) endReference = nReferences;

			for (int p = startReference; p < endReference; p++) {
				int w = p - startReference;

				if (mask[w]) {
					hits++;
					auto it = is.emplace(is.end(), Interaction<3>());
					it->d = d[w];
					it->p[0] = pt[0][w];
					it->p[1] = pt[1][w];
					it->p[2] = pt[2][w];
					it->uv[0] = t[0][w];
					it->uv[1] = t[1][w];
					it->primitiveIndex = primitiveIndex[w];
					it->nodeIndex = nodeIndex;
					it->referenceIndex = referenceOffset + p;
					it->objectIndex = aggregateIndex;
				}
			}

		} else {
			// determine closest index
			int closestIndex = -1;
			int W = std::min((int)WIDTH, nReferences - startReference);

			for (int w = 0; w < W; w++) {
				if (mask[w] && d[w] <= rtMax) {
					closestIndex = w;
					rtMax = d[w];
				}
			}

			// update interaction
			if (closestIndex != -1) {
				hits = 1;
				is[0].d = d[closestIndex];
				is[0].p[0] = pt[0][closestIndex];
				is[0].p[1] = pt[1][closestIndex];
				is[0].p[2] = pt[2][closestIndex];
				is[0].uv[0] = t[0][closestIndex];
				is[0].uv[1] = t[1][closestIndex];
				is[0].primitiveIndex = primitiveIndex[closestIndex];
				is[0].nodeIndex = nodeIndex;
				is[0].referenceIndex = referenceOffset + startReference + closestIndex;
				is[0].objectIndex = aggregateIndex;
			}
		}

		startReference += WIDTH;
	}

	return hits;
}

template<size_t WIDTH, size_t DIM, typename PrimitiveType>
inline int Mbvh<WIDTH, DIM, PrimitiveType>::intersectFromNode(Ray<DIM>& r, std::vector<Interaction<DIM>>& is,
															  int nodeStartIndex, int aggregateIndex, int& nodesVisited,
															  bool checkForOcclusion, bool recordAllHits) const
{
	int hits = 0;
	if (!recordAllHits) is.resize(1);
	BvhTraversal subtree[FCPW_MBVH_MAX_DEPTH];
	FloatP<FCPW_MBVH_BRANCHING_FACTOR> tMin, tMax;
	enokiVector<DIM> ro = enoki::gather<enokiVector<DIM>>(r.o.data(), range);
	enokiVector<DIM> rd = enoki::gather<enokiVector<DIM>>(r.d.data(), range);
	enokiVector<DIM> rinvD = enoki::gather<enokiVector<DIM>>(r.invD.data(), range);

	// push root node
	int rootIndex = aggregateIndex == this->index ? nodeStartIndex : 0;
	subtree[0].node = rootIndex;
	subtree[0].distance = minFloat;
	int stackPtr = 0;

	while (stackPtr >= 0) {
		// pop off the next node to work on
		int nodeIndex = subtree[stackPtr].node;
		float near = subtree[stackPtr].distance;
		stackPtr--;

		// if this node is further than the closest found intersection, continue
		if (!recordAllHits && near > r.tMax) continue;
		const MbvhNode<DIM>& node(flatTree[nodeIndex]);

		if (isLeafNode(node)) {
			if (vectorizedLeafType == ObjectType::LineSegments ||
				vectorizedLeafType == ObjectType::Triangles) {
				// perform vectorized intersection query
				hits += intersectPrimitives(node, leafNodes, nodeIndex, this->index, ro, rd, r.tMax, is, recordAllHits);
				nodesVisited++;

				if (hits > 0 && checkForOcclusion) {
					is.clear();
					return 1;
				}

			} else {
				// primitive type does not support vectorized intersection query,
				// perform query to each primitive one by one
				int referenceOffset = node.child[2];
				int nReferences = node.child[3];

				for (int p = 0; p < nReferences; p++) {
					int referenceIndex = referenceOffset + p;
					const PrimitiveType *prim = primitives[referenceIndex];
					nodesVisited++;

					int hit = 0;
					std::vector<Interaction<DIM>> cs;
					if (primitiveTypeIsAggregate) {
						const Aggregate<DIM> *aggregate = reinterpret_cast<const Aggregate<DIM> *>(prim);
						hit = aggregate->intersectFromNode(r, cs, nodeStartIndex, aggregateIndex,
														   nodesVisited, checkForOcclusion, recordAllHits);

					} else {
						hit = prim->intersect(r, cs, checkForOcclusion, recordAllHits);
						for (int i = 0; i < (int)cs.size(); i++) {
							cs[i].nodeIndex = nodeIndex;
							cs[i].referenceIndex = referenceIndex;
							cs[i].objectIndex = this->index;
						}
					}

					// keep the closest intersection only
					if (hit > 0) {
						if (checkForOcclusion) {
							is.clear();
							return 1;
						}

						hits += hit;
						if (recordAllHits) {
							is.insert(is.end(), cs.begin(), cs.end());

						} else {
							r.tMax = std::min(r.tMax, cs[0].d);
							is[0] = cs[0];
						}
					}
				}
			}

		} else {
			// intersect ray with boxes
			MaskP<FCPW_MBVH_BRANCHING_FACTOR> mask = intersectWideBox<FCPW_MBVH_BRANCHING_FACTOR, DIM>(
														node.boxMin, node.boxMax, ro, rinvD, r.tMax, tMin, tMax);

			// enqueue intersecting boxes in sorted order
			nodesVisited++;
			mask &= enoki::neq(node.child, maxInt);
			if (enoki::any(mask)) {
				float t = 0.0f;
				enqueueNodes(node, tMin, tMax, mask, r.tMax, t, stackPtr, subtree);
			}
		}
	}

	if (hits > 0) {
		// sort by distance and remove duplicates
		if (recordAllHits) {
			std::sort(is.begin(), is.end(), compareInteractions<DIM>);
			is = removeDuplicates<DIM>(is);
			hits = (int)is.size();

		} else {
			hits = 1;
		}

		// compute normals
		if (this->computeNormals && !primitiveTypeIsAggregate) {
			for (int i = 0; i < (int)is.size(); i++) {
				is[i].computeNormal(primitives[is[i].referenceIndex]);
			}
		}

		return hits;
	}

	return 0;
}

template<size_t WIDTH, size_t DIM, typename PrimitiveType>
inline bool findClosestPointPrimitives(const MbvhNode<DIM>& node,
									   const std::vector<MbvhLeafNode<WIDTH, DIM, PrimitiveType>>& leafNodes,
									   int nodeIndex, int aggregateIndex, const enokiVector<DIM>& sc, float& sr2,
									   Interaction<DIM>& i)
{
	std::cerr << "findClosestPointPrimitives(): WIDTH: " << WIDTH << ", DIM: " << DIM << " not supported" << std::endl;
	exit(EXIT_FAILURE);

	return false;
}

template<size_t WIDTH>
inline bool findClosestPointPrimitives(const MbvhNode<3>& node,
									   const std::vector<MbvhLeafNode<WIDTH, 3, LineSegment>>& leafNodes,
									   int nodeIndex, int aggregateIndex, const enokiVector3& sc, float& sr2,
									   Interaction<3>& i)
{
	int leafOffset = -node.child[0] - 1;
	int nLeafs = node.child[1];
	int referenceOffset = node.child[2];
	int nReferences = node.child[3];
	int startReference = 0;
	bool found = false;

	for (int l = 0; l < nLeafs; l++) {
		// perform vectorized closest point query
		Vector3P<WIDTH> pt;
		FloatP<WIDTH> t;
		int leafIndex = leafOffset + l;
		const Vector3P<WIDTH>& pa = leafNodes[leafIndex].positions[0];
		const Vector3P<WIDTH>& pb = leafNodes[leafIndex].positions[1];
		const IntP<WIDTH>& primitiveIndex = leafNodes[leafIndex].primitiveIndex;
		FloatP<WIDTH> d = findClosestPointWideLineSegment<WIDTH>(pa, pb, sc, pt, t);
		FloatP<WIDTH> d2 = d*d;

		// determine closest index
		int closestIndex = -1;
		int W = std::min((int)WIDTH, nReferences - startReference);

		for (int w = 0; w < W; w++) {
			if (d2[w] <= sr2) {
				closestIndex = w;
				sr2 = d2[w];
			}
		}

		// update interaction
		if (closestIndex != -1) {
			i.d = d[closestIndex];
			i.p[0] = pt[0][closestIndex];
			i.p[1] = pt[1][closestIndex];
			i.p[2] = pt[2][closestIndex];
			i.uv[0] = t[closestIndex];
			i.uv[1] = -1;
			i.primitiveIndex = primitiveIndex[closestIndex];
			i.nodeIndex = nodeIndex;
			i.referenceIndex = referenceOffset + startReference + closestIndex;
			i.objectIndex = aggregateIndex;
			found = true;
		}

		startReference += WIDTH;
	}

	return found;
}

template<size_t WIDTH>
inline bool findClosestPointPrimitives(const MbvhNode<3>& node,
									   const std::vector<MbvhLeafNode<WIDTH, 3, Triangle>>& leafNodes,
									   int nodeIndex, int aggregateIndex, const enokiVector3& sc, float& sr2,
									   Interaction<3>& i)
{
	int leafOffset = -node.child[0] - 1;
	int nLeafs = node.child[1];
	int referenceOffset = node.child[2];
	int nReferences = node.child[3];
	int startReference = 0;
	bool found = false;

	for (int l = 0; l < nLeafs; l++) {
		// perform vectorized closest point query
		Vector3P<WIDTH> pt;
		Vector2P<WIDTH> t;
		int leafIndex = leafOffset + l;
		const Vector3P<WIDTH>& pa = leafNodes[leafIndex].positions[0];
		const Vector3P<WIDTH>& pb = leafNodes[leafIndex].positions[1];
		const Vector3P<WIDTH>& pc = leafNodes[leafIndex].positions[2];
		const IntP<WIDTH>& primitiveIndex = leafNodes[leafIndex].primitiveIndex;
		FloatP<WIDTH> d = findClosestPointWideTriangle<WIDTH>(pa, pb, pc, sc, pt, t);
		FloatP<WIDTH> d2 = d*d;

		// determine closest index
		int closestIndex = -1;
		int W = std::min((int)WIDTH, nReferences - startReference);

		for (int w = 0; w < W; w++) {
			if (d2[w] <= sr2) {
				closestIndex = w;
				sr2 = d2[w];
			}
		}

		// update interaction
		if (closestIndex != -1) {
			i.d = d[closestIndex];
			i.p[0] = pt[0][closestIndex];
			i.p[1] = pt[1][closestIndex];
			i.p[2] = pt[2][closestIndex];
			i.uv[0] = t[0][closestIndex];
			i.uv[1] = t[1][closestIndex];
			i.primitiveIndex = primitiveIndex[closestIndex];
			i.nodeIndex = nodeIndex;
			i.referenceIndex = referenceOffset + startReference + closestIndex;
			i.objectIndex = aggregateIndex;
			found = true;
		}

		startReference += WIDTH;
	}

	return found;
}

template<size_t WIDTH, size_t DIM, typename PrimitiveType>
inline bool Mbvh<WIDTH, DIM, PrimitiveType>::findClosestPointFromNode(BoundingSphere<DIM>& s, Interaction<DIM>& i,
																	  int nodeStartIndex, int aggregateIndex,
																	  const Vector<DIM>& boundaryHint, int& nodesVisited) const
{
	// TODO: use direction to boundary guess
	bool notFound = true;
	BvhTraversal subtree[FCPW_MBVH_MAX_DEPTH];
	FloatP<FCPW_MBVH_BRANCHING_FACTOR> d2Min, d2Max;
	enokiVector<DIM> sc = enoki::gather<enokiVector<DIM>>(s.c.data(), range);

	// push root node
	int rootIndex = aggregateIndex == this->index ? nodeStartIndex : 0;
	subtree[0].node = rootIndex;
	subtree[0].distance = minFloat;
	int stackPtr = 0;

	while (stackPtr >= 0) {
		// pop off the next node to work on
		int nodeIndex = subtree[stackPtr].node;
		float near = subtree[stackPtr].distance;
		stackPtr--;

		// if this node is further than the closest found primitive, continue
		if (near > s.r2) continue;
		const MbvhNode<DIM>& node(flatTree[nodeIndex]);

		if (isLeafNode(node)) {
			if (vectorizedLeafType == ObjectType::LineSegments ||
				vectorizedLeafType == ObjectType::Triangles) {
				// perform vectorized closest point query to triangle
				bool found = findClosestPointPrimitives(node, leafNodes, nodeIndex, this->index, sc, s.r2, i);
				if (found) notFound = false;
				nodesVisited++;

			} else {
				// primitive type does not support vectorized closest point query,
				// perform query to each primitive one by one
				int referenceOffset = node.child[2];
				int nReferences = node.child[3];

				for (int p = 0; p < nReferences; p++) {
					int referenceIndex = referenceOffset + p;
					const PrimitiveType *prim = primitives[referenceIndex];
					nodesVisited++;

					bool found = false;
					Interaction<DIM> c;
					if (primitiveTypeIsAggregate) {
						const Aggregate<DIM> *aggregate = reinterpret_cast<const Aggregate<DIM> *>(prim);
						found = aggregate->findClosestPointFromNode(s, c, nodeStartIndex, aggregateIndex,
																	boundaryHint, nodesVisited);

					} else {
						found = prim->findClosestPoint(s, c);
						c.nodeIndex = nodeIndex;
						c.referenceIndex = referenceIndex;
						c.objectIndex = this->index;
					}

					// keep the closest point only
					if (found) {
						notFound = false;
						s.r2 = std::min(s.r2, c.d*c.d);
						i = c;
					}
				}
			}

		} else {
			// overlap sphere with boxes
			MaskP<FCPW_MBVH_BRANCHING_FACTOR> mask = overlapWideBox<FCPW_MBVH_BRANCHING_FACTOR, DIM>(
																node.boxMin, node.boxMax, sc, s.r2, d2Min, d2Max);

			// enqueue overlapping boxes in sorted order
			nodesVisited++;
			mask &= enoki::neq(node.child, maxInt);
			if (enoki::any(mask)) {
				enqueueNodes(node, d2Min, d2Max, mask, s.r2, s.r2, stackPtr, subtree);
			}
		}
	}

	if (!notFound) {
		// compute normal
		if (this->computeNormals && !primitiveTypeIsAggregate) {
			i.computeNormal(primitives[i.referenceIndex]);
		}

		return true;
	}

	return false;
}

} // namespace fcpw
