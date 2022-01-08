#include "bvh_utils.hpp"
#include <bvh/sweep_sah_builder.hpp>
#include <bvh/binned_sah_builder.hpp>

bvh::Bvh<float> BuildBVH(std::vector<bvh::Triangle<float>> &triangles, int iteration)
{
	bvh::Bvh<float> bvh;

    auto [bboxes, centers] = bvh::compute_bounding_boxes_and_centers(triangles.data(), triangles.size());
    auto global_bbox = bvh::compute_bounding_boxes_union(bboxes.get(), triangles.size());

    // Create an acceleration data structure on the primitives
	if(iteration == 0)
	{
		bvh::BinnedSahBuilder<bvh::Bvh<float>, 5> builder(bvh);
		builder.max_depth = 3;
		builder.build(global_bbox, bboxes.get(), centers.get(), triangles.size());
	}
	else if(iteration == 1)
	{
		bvh::SweepSahBuilder<bvh::Bvh<float>> builder(bvh);
		builder.max_depth = 3;
		builder.build(global_bbox, bboxes.get(), centers.get(), triangles.size());
	}
	else
	{
		bvh::BinnedSahBuilder<bvh::Bvh<float>, 3> builder(bvh);
		builder.max_depth = 4;
		builder.build(global_bbox, bboxes.get(), centers.get(), triangles.size());
	}

	return bvh;
}