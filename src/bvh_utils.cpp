#include "bvh_utils.hpp"
#include <bvh/sweep_sah_builder.hpp>

bvh::Bvh<float> BuildBVH(std::vector<bvh::Triangle<float>> &triangles)
{
	bvh::Bvh<float> bvh;

    auto [bboxes, centers] = bvh::compute_bounding_boxes_and_centers(triangles.data(), triangles.size());
    auto global_bbox = bvh::compute_bounding_boxes_union(bboxes.get(), triangles.size());

    // Create an acceleration data structure on the primitives
    bvh::SweepSahBuilder<bvh::Bvh<float>> builder(bvh);
	builder.max_depth = 5; // TODO: play around with this parameter
    builder.build(global_bbox, bboxes.get(), centers.get(), triangles.size());

	return bvh;
}