#include "Scene.h"

PoissonEquation BoundaryScene(bool importancesampling)
{
	string file = "../examples/scenes/circle_boundary.obj";
	ptrSourceTerm source = make_shared<EmptySource>();

	auto bd = Boundary::ReadObjCurve(file);
	auto& points = bd.points;

	double intensity = 1.5;
	Vec3 a{ 0.498, 0.345, 0.686 };
	Vec3 b{ 0.39, 0.77, 0.92 };
	Vec3 c{ 0.91, 0.302, 0.541 };
	Vec3 d{ 0.996, 0.702, 0.149 };
	vector<Vec3> ColorSet = {
		intensity * a, 
		intensity * b, 
		intensity * c, 
		intensity * d
	};

	ptrBoundary boundary = make_shared<SegmentBoundaryNew>(points, ColorSet, 4, 12, importancesampling);
	assert(boundary->HasBoundaryValue() == true);
	assert(source->HasSource() == false);
	
	return PoissonEquation(boundary, source);
}

PoissonEquation SourceScene()
{
	string file = "../examples/scenes/curve_boundary.obj";
	auto bd = Boundary::ReadObjCurve(file);
	auto& points = bd.points;
	
	std::cout << "Using boundary with line segment: " << points.size() << std::endl;
	double intensity = 32.0;
	DiskSource::SphereSource R{ {-ScreenSize * 0.3, 0.0}, ScreenSize * 0.005, {intensity * 10000.0, 0.0, 0.0} };
	DiskSource::SphereSource G{ {-ScreenSize * 0.0, 0.0}, ScreenSize * 0.05, {0.0, intensity * 100.0, 0.0} };
	DiskSource::SphereSource B{ {ScreenSize * 0.4, -0.05}, ScreenSize * 0.3, {0.0, 0.0, intensity * 2.77777} };

	std::vector<DiskSource::SphereSource> spheres = { R, G, B };
	ptrSourceTerm source = make_shared<DiskSource>(spheres);
	ptrBoundary   boundary = make_shared<Boundary>(points);
	assert(boundary->HasBoundaryValue() == false);
	
	return PoissonEquation(boundary, source);
}

PoissonEquation PointSourceInSphere()
{
	auto bd = Boundary::ReadObjCurve("../examples/scenes/curve_boundary.obj");

	auto& points = bd.points;
	auto& index = bd.index;
	ptrBoundary boundary = make_shared<Boundary>(points, index);

	double intensity = 1;
	PointSource ps{ {0.0, 0.7}, {intensity, intensity, 0.0} };
	vector<PointSource> source_list{ ps };
	ptrSourceTerm source = make_shared<DiscreteSource>(source_list);
	assert(boundary->HasBoundaryValue() == false);
	return PoissonEquation(boundary, source);
}


void AddLine(std::vector<Vec2>& points, std::vector<int>& index, std::vector<Vec3>& frontColor, std::vector<Vec3>& backColor,
	Vec2 a, Vec2 b, Vec3 c1, Vec3 c2)
{

	index.push_back(points.size());
	index.push_back(points.size() + 1);

	points.push_back(a);
	points.push_back(b);

	frontColor.push_back(c1);
	frontColor.push_back(c1);

	backColor.push_back(c2);
	backColor.push_back(c2);
}

void AddLine(std::vector<Vec2>& points, std::vector<int>& index, std::vector<Vec3>& frontColor, std::vector<Vec3>& backColor,
	Vec2 a, Vec2 b, Vec3 cu0, Vec3 cv0, Vec3 cu1, Vec3 cv1)
{
	
	Vec3 r{ 1.0, 0.0, 0.0 };
	Vec3 g{ 0.0, 1.0, 0.0 };

	index.push_back(points.size());
	index.push_back(points.size() + 1);

	points.push_back(a);
	points.push_back(b);

	frontColor.push_back(cu0);
	frontColor.push_back(cu1);

	backColor.push_back(cv0);
	backColor.push_back(cv1);
}

PoissonEquation BugDiffusionCurve()
{
	std::vector<Vec2> points;
	std::vector<int> index;
	std::vector<Vec3> frontColor;
	std::vector<Vec3> backColor;
	// Copy from https://www.shadertoy.com/view/WdXfzl
	{
		AddLine(points, index, frontColor, backColor, Vec2(0.032566, 0.636419), Vec2(-0.140558, 0.554919), Vec3(0.802, 0.563, 0.443), Vec3(0.865, 0.792, 0.690));
		AddLine(points, index, frontColor, backColor, Vec2(0.157917, 0.667506), Vec2(0.032586, 0.636408), Vec3(0.824, 0.575, 0.453), Vec3(0.869, 0.804, 0.702));
		AddLine(points, index, frontColor, backColor, Vec2(0.292774, 0.683634), Vec2(0.157637, 0.667307), Vec3(0.849, 0.600, 0.467), Vec3(0.871, 0.804, 0.702));
		AddLine(points, index, frontColor, backColor, Vec2(0.462459, 0.681116), Vec2(0.293026, 0.683576), Vec3(0.890, 0.647, 0.488), Vec3(0.871, 0.804, 0.700));
		AddLine(points, index, frontColor, backColor, Vec2(0.532097, 0.663221), Vec2(0.462281, 0.681257), Vec3(0.931, 0.692, 0.510), Vec3(0.871, 0.804, 0.698));
		AddLine(points, index, frontColor, backColor, Vec2(0.679623, 0.589789), Vec2(0.532148, 0.663353), Vec3(0.951, 0.712, 0.518), Vec3(0.871, 0.804, 0.698));
		AddLine(points, index, frontColor, backColor, Vec2(0.737901, 0.539194), Vec2(0.680024, 0.589419), Vec3(0.951, 0.712, 0.512), Vec3(0.871, 0.804, 0.698));
		AddLine(points, index, frontColor, backColor, Vec2(0.816252, 0.438381), Vec2(0.737331, 0.539616), Vec3(0.933, 0.700, 0.502), Vec3(0.871, 0.804, 0.696));
		AddLine(points, index, frontColor, backColor, Vec2(0.855568, 0.362220), Vec2(0.816666, 0.437574), Vec3(0.920, 0.684, 0.486), Vec3(0.871, 0.802, 0.694));
		AddLine(points, index, frontColor, backColor, Vec2(0.914063, 0.195570), Vec2(0.855412, 0.362903), Vec3(0.904, 0.667, 0.467), Vec3(0.871, 0.800, 0.694));
		AddLine(points, index, frontColor, backColor, Vec2(0.906394, -0.050806), Vec2(0.914184, 0.195399), Vec3(0.876, 0.637, 0.437), Vec3(0.871, 0.800, 0.692));
		AddLine(points, index, frontColor, backColor, Vec2(0.847684, -0.254043), Vec2(0.906304, -0.051022), Vec3(0.841, 0.600, 0.402), Vec3(0.871, 0.800, 0.688));
		AddLine(points, index, frontColor, backColor, Vec2(0.753134, -0.419097), Vec2(0.847548, -0.253827), Vec3(0.808, 0.571, 0.373), Vec3(0.871, 0.798, 0.686));
		AddLine(points, index, frontColor, backColor, Vec2(0.655326, -0.537575), Vec2(0.752991, -0.419390), Vec3(0.796, 0.557, 0.357), Vec3(0.871, 0.796, 0.686), Vec3(0.698, 0.439, 0.271), Vec3(0.871, 0.796, 0.686));
		AddLine(points, index, frontColor, backColor, Vec2(0.589686, -0.601779), Vec2(0.655427, -0.537635), Vec3(0.698, 0.439, 0.271), Vec3(0.871, 0.796, 0.686), Vec3(0.592, 0.298, 0.149), Vec3(0.871, 0.796, 0.686));
		AddLine(points, index, frontColor, backColor, Vec2(0.485769, -0.673529), Vec2(0.589993, -0.601259), Vec3(0.553, 0.269, 0.131), Vec3(0.871, 0.796, 0.684));
		AddLine(points, index, frontColor, backColor, Vec2(0.420683, -0.695361), Vec2(0.485736, -0.673833), Vec3(0.506, 0.247, 0.124), Vec3(0.876, 0.806, 0.692));
		AddLine(points, index, frontColor, backColor, Vec2(0.289074, -0.705976), Vec2(0.420341, -0.695230), Vec3(0.482, 0.275, 0.155), Vec3(0.876, 0.806, 0.692));
		AddLine(points, index, frontColor, backColor, Vec2(0.261794, -0.694878), Vec2(0.289218, -0.706199), Vec3(0.459, 0.284, 0.173), Vec3(0.871, 0.796, 0.680));
		AddLine(points, index, frontColor, backColor, Vec2(0.234336, -0.652438), Vec2(0.261805, -0.694874), Vec3(0.443, 0.249, 0.153), Vec3(0.871, 0.796, 0.678));
		AddLine(points, index, frontColor, backColor, Vec2(0.229172, -0.613117), Vec2(0.234298, -0.652477), Vec3(0.420, 0.184, 0.116), Vec3(0.871, 0.796, 0.678));
		AddLine(points, index, frontColor, backColor, Vec2(0.211059, -0.573956), Vec2(0.229373, -0.612721), Vec3(0.404, 0.145, 0.094), Vec3(0.871, 0.796, 0.678), Vec3(0.384, 0.090, 0.059), Vec3(0.631, 0.506, 0.447));
		AddLine(points, index, frontColor, backColor, Vec2(0.794264, 0.332819), Vec2(0.820637, 0.237829), Vec3(0.961, 0.525, 0.318), Vec3(0.941, 0.394, 0.175));
		AddLine(points, index, frontColor, backColor, Vec2(0.791799, 0.406427), Vec2(0.794099, 0.333239), Vec3(0.967, 0.565, 0.365), Vec3(0.941, 0.388, 0.163));
		AddLine(points, index, frontColor, backColor, Vec2(0.773116, 0.456365), Vec2(0.791677, 0.406208), Vec3(0.973, 0.596, 0.406), Vec3(0.941, 0.382, 0.153));
		AddLine(points, index, frontColor, backColor, Vec2(0.691214, 0.542967), Vec2(0.772875, 0.456604), Vec3(0.980, 0.631, 0.455), Vec3(0.941, 0.375, 0.139));
		AddLine(points, index, frontColor, backColor, Vec2(0.584748, 0.612130), Vec2(0.691856, 0.543066), Vec3(0.988, 0.675, 0.510), Vec3(0.941, 0.367, 0.124));
		AddLine(points, index, frontColor, backColor, Vec2(0.523270, 0.633009), Vec2(0.583850, 0.612122), Vec3(0.994, 0.702, 0.543), Vec3(0.941, 0.361, 0.114));
		AddLine(points, index, frontColor, backColor, Vec2(0.416081, 0.649827), Vec2(0.523622, 0.632948), Vec3(0.996, 0.710, 0.553), Vec3(0.941, 0.357, 0.110), Vec3(0.965, 0.596, 0.420), Vec3(0.941, 0.353, 0.094));
		AddLine(points, index, frontColor, backColor, Vec2(0.328754, 0.652317), Vec2(0.416523, 0.649811), Vec3(0.955, 0.573, 0.390), Vec3(0.941, 0.349, 0.090));
		AddLine(points, index, frontColor, backColor, Vec2(0.152437, 0.628754), Vec2(0.328509, 0.652168), Vec3(0.945, 0.549, 0.361), Vec3(0.941, 0.345, 0.086), Vec3(0.918, 0.671, 0.486), Vec3(0.929, 0.404, 0.192));
		AddLine(points, index, frontColor, backColor, Vec2(-0.058755, 0.562422), Vec2(0.152170, 0.628817), Vec3(0.918, 0.671, 0.486), Vec3(0.929, 0.404, 0.192), Vec3(0.847, 0.643, 0.471), Vec3(0.910, 0.478, 0.325));
		AddLine(points, index, frontColor, backColor, Vec2(-0.152258, 0.413501), Vec2(-0.073990, 0.418056), Vec3(0.322, 0.165, 0.318), Vec3(0.839, 0.357, 0.294), Vec3(0.325, 0.161, 0.341), Vec3(0.875, 0.427, 0.400));
		AddLine(points, index, frontColor, backColor, Vec2(-0.166656, 0.426903), Vec2(-0.152256, 0.413325), Vec3(0.325, 0.161, 0.341), Vec3(0.875, 0.427, 0.400), Vec3(0.329, 0.157, 0.361), Vec3(0.929, 0.510, 0.518));
		AddLine(points, index, frontColor, backColor, Vec2(-0.150301, 0.459101), Vec2(-0.167019, 0.427044), Vec3(0.327, 0.155, 0.355), Vec3(0.902, 0.504, 0.522));
		AddLine(points, index, frontColor, backColor, Vec2(-0.118651, 0.485569), Vec2(-0.150151, 0.459146), Vec3(0.322, 0.151, 0.329), Vec3(0.892, 0.510, 0.522));
		AddLine(points, index, frontColor, backColor, Vec2(-0.002352, 0.524810), Vec2(-0.118560, 0.485419), Vec3(0.302, 0.141, 0.253), Vec3(0.918, 0.525, 0.484));
		AddLine(points, index, frontColor, backColor, Vec2(0.031055, 0.523623), Vec2(-0.002648, 0.525035), Vec3(0.329, 0.161, 0.216), Vec3(0.927, 0.531, 0.443));
		AddLine(points, index, frontColor, backColor, Vec2(0.698487, 0.039536), Vec2(0.691611, -0.011391), Vec3(0.241, 0.084, 0.069), Vec3(0.894, 0.271, 0.043));
		AddLine(points, index, frontColor, backColor, Vec2(0.719461, 0.087611), Vec2(0.698093, 0.039144), Vec3(0.208, 0.076, 0.112), Vec3(0.892, 0.271, 0.041));
		AddLine(points, index, frontColor, backColor, Vec2(0.756735, 0.118077), Vec2(0.719793, 0.087918), Vec3(0.180, 0.078, 0.149), Vec3(0.890, 0.271, 0.037));
		AddLine(points, index, frontColor, backColor, Vec2(0.781480, 0.121070), Vec2(0.756822, 0.117813), Vec3(0.208, 0.100, 0.151), Vec3(0.890, 0.271, 0.035));
		AddLine(points, index, frontColor, backColor, Vec2(0.801014, 0.097611), Vec2(0.781005, 0.120993), Vec3(0.220, 0.114, 0.149), Vec3(0.888, 0.269, 0.033));
		AddLine(points, index, frontColor, backColor, Vec2(0.823724, 0.024839), Vec2(0.801409, 0.097493), Vec3(0.210, 0.129, 0.149), Vec3(0.886, 0.267, 0.029));
		AddLine(points, index, frontColor, backColor, Vec2(0.823815, -0.104922), Vec2(0.823541, 0.024940), Vec3(0.247, 0.180, 0.135), Vec3(0.882, 0.265, 0.024));
		AddLine(points, index, frontColor, backColor, Vec2(0.812986, -0.134971), Vec2(0.823897, -0.104904), Vec3(0.275, 0.206, 0.120), Vec3(0.878, 0.263, 0.018));
		AddLine(points, index, frontColor, backColor, Vec2(0.781477, -0.159956), Vec2(0.813047, -0.134843), Vec3(0.275, 0.182, 0.102), Vec3(0.876, 0.263, 0.014));
		AddLine(points, index, frontColor, backColor, Vec2(0.742899, -0.136473), Vec2(0.781414, -0.160083), Vec3(0.273, 0.161, 0.084), Vec3(0.873, 0.261, 0.010));
		AddLine(points, index, frontColor, backColor, Vec2(0.707845, -0.099381), Vec2(0.742671, -0.136282), Vec3(0.278, 0.145, 0.071), Vec3(0.871, 0.259, 0.006));
		AddLine(points, index, frontColor, backColor, Vec2(0.692791, -0.057184), Vec2(0.707941, -0.099276), Vec3(0.278, 0.127, 0.057), Vec3(0.869, 0.259, 0.002));
		AddLine(points, index, frontColor, backColor, Vec2(0.691497, -0.011596), Vec2(0.692977, -0.057240), Vec3(0.269, 0.114, 0.049), Vec3(0.859, 0.257, 0.000));
		AddLine(points, index, frontColor, backColor, Vec2(0.649558, -0.424193), Vec2(0.570243, -0.515577), Vec3(0.394, 0.220, 0.129), Vec3(0.763, 0.245, 0.125));
		AddLine(points, index, frontColor, backColor, Vec2(0.691564, -0.410217), Vec2(0.649794, -0.424293), Vec3(0.408, 0.216, 0.118), Vec3(0.800, 0.247, 0.122), Vec3(0.635, 0.341, 0.208), Vec3(0.773, 0.318, 0.176));
		AddLine(points, index, frontColor, backColor, Vec2(0.696625, -0.423080), Vec2(0.691171, -0.410166), Vec3(0.635, 0.341, 0.208), Vec3(0.773, 0.318, 0.176), Vec3(0.569, 0.345, 0.220), Vec3(0.749, 0.416, 0.251));
		AddLine(points, index, frontColor, backColor, Vec2(0.689415, -0.442915), Vec2(0.697144, -0.422830), Vec3(0.529, 0.343, 0.227), Vec3(0.729, 0.441, 0.273));
		AddLine(points, index, frontColor, backColor, Vec2(0.593770, -0.551001), Vec2(0.689147, -0.443398), Vec3(0.441, 0.322, 0.225), Vec3(0.688, 0.429, 0.271));
		AddLine(points, index, frontColor, backColor, Vec2(0.555892, -0.565232), Vec2(0.593883, -0.550881), Vec3(0.431, 0.282, 0.198), Vec3(0.665, 0.351, 0.220));
		AddLine(points, index, frontColor, backColor, Vec2(0.551358, -0.559085), Vec2(0.555731, -0.565064), Vec3(0.471, 0.263, 0.180), Vec3(0.663, 0.310, 0.192), Vec3(0.345, 0.263, 0.192), Vec3(0.659, 0.271, 0.169));
		AddLine(points, index, frontColor, backColor, Vec2(0.570349, -0.515707), Vec2(0.551420, -0.559235), Vec3(0.343, 0.257, 0.188), Vec3(0.661, 0.257, 0.157));
		AddLine(points, index, frontColor, backColor, Vec2(0.139656, 0.051930), Vec2(0.125174, -0.011939), Vec3(0.212, 0.176, 0.188), Vec3(0.773, 0.251, 0.153), Vec3(0.322, 0.298, 0.314), Vec3(0.831, 0.376, 0.290));
		AddLine(points, index, frontColor, backColor, Vec2(0.156524, 0.072534), Vec2(0.139337, 0.052148), Vec3(0.322, 0.298, 0.314), Vec3(0.831, 0.376, 0.290), Vec3(0.455, 0.388, 0.420), Vec3(0.863, 0.510, 0.447));
		AddLine(points, index, frontColor, backColor, Vec2(0.207624, 0.092959), Vec2(0.156443, 0.072549), Vec3(0.455, 0.388, 0.420), Vec3(0.863, 0.510, 0.447), Vec3(0.608, 0.612, 0.635), Vec3(0.902, 0.635, 0.588));
		AddLine(points, index, frontColor, backColor, Vec2(0.277523, 0.089856), Vec2(0.208031, 0.092954), Vec3(0.608, 0.612, 0.635), Vec3(0.902, 0.635, 0.588), Vec3(0.792, 0.820, 0.851), Vec3(0.945, 0.776, 0.753));
		AddLine(points, index, frontColor, backColor, Vec2(0.342574, 0.060277), Vec2(0.277521, 0.089925), Vec3(0.792, 0.820, 0.851), Vec3(0.945, 0.776, 0.753), Vec3(0.851, 0.867, 0.894), Vec3(0.988, 0.914, 0.910));
		AddLine(points, index, frontColor, backColor, Vec2(0.395896, 0.011487), Vec2(0.342325, 0.060273), Vec3(0.851, 0.867, 0.894), Vec3(0.988, 0.914, 0.910), Vec3(0.816, 0.769, 0.788), Vec3(0.973, 0.792, 0.769));
		AddLine(points, index, frontColor, backColor, Vec2(0.426593, -0.056929), Vec2(0.395983, 0.010998), Vec3(0.816, 0.769, 0.788), Vec3(0.973, 0.792, 0.769), Vec3(0.784, 0.690, 0.702), Vec3(0.953, 0.675, 0.624));
		AddLine(points, index, frontColor, backColor, Vec2(0.429756, -0.097479), Vec2(0.426646, -0.056343), Vec3(0.784, 0.690, 0.702), Vec3(0.955, 0.676, 0.627));
		AddLine(points, index, frontColor, backColor, Vec2(0.211977, 0.039860), Vec2(0.152329, 0.066394), Vec3(0.251, 0.255, 0.322), Vec3(0.573, 0.522, 0.557), Vec3(0.239, 0.251, 0.325), Vec3(0.612, 0.620, 0.655));
		AddLine(points, index, frontColor, backColor, Vec2(0.253875, -0.009396), Vec2(0.212258, 0.039555), Vec3(0.224, 0.235, 0.298), Vec3(0.627, 0.651, 0.694));
		AddLine(points, index, frontColor, backColor, Vec2(0.304442, -0.112422), Vec2(0.254010, -0.009088), Vec3(0.190, 0.196, 0.251), Vec3(0.629, 0.653, 0.704));
		AddLine(points, index, frontColor, backColor, Vec2(0.333171, -0.133931), Vec2(0.303949, -0.112429), Vec3(0.178, 0.169, 0.220), Vec3(0.612, 0.620, 0.671));
		AddLine(points, index, frontColor, backColor, Vec2(0.374078, -0.127448), Vec2(0.333105, -0.133826), Vec3(0.222, 0.190, 0.220), Vec3(0.629, 0.629, 0.675));
		AddLine(points, index, frontColor, backColor, Vec2(0.429594, -0.097757), Vec2(0.374129, -0.127644), Vec3(0.278, 0.229, 0.237), Vec3(0.678, 0.659, 0.694));
		AddLine(points, index, frontColor, backColor, Vec2(0.128499, -0.076818), Vec2(0.113227, -0.128679), Vec3(0.131, 0.071, 0.118), Vec3(0.986, 0.927, 0.737));
		AddLine(points, index, frontColor, backColor, Vec2(0.214776, -0.207363), Vec2(0.168035, -0.203218), Vec3(0.982, 0.796, 0.496), Vec3(0.124, 0.039, 0.092));
		AddLine(points, index, frontColor, backColor, Vec2(0.238606, -0.218984), Vec2(0.215027, -0.207500), Vec3(0.990, 0.782, 0.447), Vec3(0.153, 0.033, 0.057));
		AddLine(points, index, frontColor, backColor, Vec2(0.252587, -0.370868), Vec2(0.218651, -0.371118), Vec3(0.441, 0.100, 0.059), Vec3(0.845, 0.380, 0.137));
		AddLine(points, index, frontColor, backColor, Vec2(0.276234, -0.360153), Vec2(0.252950, -0.370523), Vec3(0.496, 0.124, 0.057), Vec3(0.847, 0.412, 0.163));
		AddLine(points, index, frontColor, backColor, Vec2(0.296842, -0.316514), Vec2(0.275936, -0.360588), Vec3(0.533, 0.141, 0.055), Vec3(0.847, 0.431, 0.180), Vec3(0.643, 0.192, 0.051), Vec3(0.910, 0.561, 0.271));
		AddLine(points, index, frontColor, backColor, Vec2(0.296875, -0.238281), Vec2(0.296875, -0.316406), Vec3(0.643, 0.192, 0.051), Vec3(0.910, 0.561, 0.271), Vec3(0.741, 0.239, 0.047), Vec3(0.953, 0.678, 0.357));
		AddLine(points, index, frontColor, backColor, Vec2(-0.018745, -0.009492), Vec2(-0.023346, -0.023500), Vec3(0.984, 0.665, 0.482), Vec3(0.824, 0.235, 0.180));
		AddLine(points, index, frontColor, backColor, Vec2(0.004475, 0.004419), Vec2(-0.019006, -0.009652), Vec3(0.976, 0.686, 0.506), Vec3(0.798, 0.210, 0.153));
		AddLine(points, index, frontColor, backColor, Vec2(0.078024, -0.000098), Vec2(0.004545, 0.004548), Vec3(0.935, 0.696, 0.480), Vec3(0.792, 0.229, 0.122));
		AddLine(points, index, frontColor, backColor, Vec2(0.129006, -0.035229), Vec2(0.078225, -0.000073), Vec3(0.902, 0.686, 0.447), Vec3(0.788, 0.243, 0.102), Vec3(0.937, 0.769, 0.545), Vec3(0.769, 0.255, 0.059));
		AddLine(points, index, frontColor, backColor, Vec2(0.214680, -0.370938), Vec2(0.167824, -0.202999), Vec3(0.263, 0.220, 0.243), Vec3(0.941, 0.643, 0.373));
		AddLine(points, index, frontColor, backColor, Vec2(0.117434, -0.128738), Vec2(-0.023211, -0.023293), Vec3(0.718, 0.698, 0.690), Vec3(0.984, 0.867, 0.718));
		AddLine(points, index, frontColor, backColor, Vec2(-0.027606, -0.023401), Vec2(-0.250232, 0.058604), Vec3(0.369, 0.376, 0.439), Vec3(0.824, 0.357, 0.349));
		AddLine(points, index, frontColor, backColor, Vec2(0.224314, -0.490840), Vec2(0.214794, -0.371444), Vec3(0.110, 0.076, 0.102), Vec3(0.553, 0.102, 0.061));
		AddLine(points, index, frontColor, backColor, Vec2(0.210828, -0.578506), Vec2(0.224257, -0.490867), Vec3(0.118, 0.094, 0.114), Vec3(0.467, 0.108, 0.084));
		AddLine(points, index, frontColor, backColor, Vec2(-0.738216, 0.577331), Vec2(-0.835883, 0.550764), Vec3(0.647, 0.639, 0.914), Vec3(0.859, 0.780, 0.682));
		AddLine(points, index, frontColor, backColor, Vec2(-0.428590, 0.601696), Vec2(-0.738185, 0.577395), Vec3(0.698, 0.680, 0.884), Vec3(0.865, 0.778, 0.667));
		AddLine(points, index, frontColor, backColor, Vec2(-0.257866, 0.593737), Vec2(-0.428754, 0.601618), Vec3(0.763, 0.733, 0.845), Vec3(0.876, 0.775, 0.645));
		AddLine(points, index, frontColor, backColor, Vec2(-0.136723, 0.554703), Vec2(-0.257809, 0.593759), Vec3(0.808, 0.769, 0.820), Vec3(0.878, 0.775, 0.657));
		AddLine(points, index, frontColor, backColor, Vec2(-0.910838, 0.948616), Vec2(-0.980469, 0.968751), Vec3(0.782, 0.753, 0.910), Vec3(0.486, 0.422, 0.769));
		AddLine(points, index, frontColor, backColor, Vec2(-0.849120, 0.905236), Vec2(-0.910830, 0.948608), Vec3(0.771, 0.741, 0.908), Vec3(0.494, 0.431, 0.773));
		AddLine(points, index, frontColor, backColor, Vec2(-0.805151, 0.844085), Vec2(-0.849092, 0.905222), Vec3(0.751, 0.724, 0.904), Vec3(0.494, 0.431, 0.773));
		AddLine(points, index, frontColor, backColor, Vec2(-0.792960, 0.808567), Vec2(-0.805191, 0.844089), Vec3(0.741, 0.714, 0.902), Vec3(0.494, 0.431, 0.773));
		AddLine(points, index, frontColor, backColor, Vec2(-0.660730, 0.350115), Vec2(-0.687380, 0.324080), Vec3(0.569, 0.539, 0.929), Vec3(0.625, 0.612, 0.963));
		AddLine(points, index, frontColor, backColor, Vec2(-0.581342, 0.390737), Vec2(-0.660931, 0.350300), Vec3(0.610, 0.565, 0.941), Vec3(0.671, 0.641, 0.963));
		AddLine(points, index, frontColor, backColor, Vec2(-0.519169, 0.405706), Vec2(-0.581260, 0.390541), Vec3(0.653, 0.590, 0.953), Vec3(0.722, 0.671, 0.963));
		AddLine(points, index, frontColor, backColor, Vec2(-0.325640, 0.410142), Vec2(-0.519014, 0.405733), Vec3(0.708, 0.624, 0.969), Vec3(0.784, 0.706, 0.965));
		AddLine(points, index, frontColor, backColor, Vec2(-0.265662, 0.386779), Vec2(-0.325886, 0.410329), Vec3(0.767, 0.661, 0.982), Vec3(0.847, 0.741, 0.965));
		AddLine(points, index, frontColor, backColor, Vec2(-0.890704, 0.394153), Vec2(-0.996080, 0.374882), Vec3(0.576, 0.569, 0.918), Vec3(0.635, 0.637, 0.951));
		AddLine(points, index, frontColor, backColor, Vec2(-0.808329, 0.393799), Vec2(-0.890754, 0.394278), Vec3(0.575, 0.569, 0.922), Vec3(0.635, 0.635, 0.955));
		AddLine(points, index, frontColor, backColor, Vec2(-0.746093, 0.374902), Vec2(-0.808295, 0.393687), Vec3(0.573, 0.569, 0.924), Vec3(0.635, 0.629, 0.961));
		AddLine(points, index, frontColor, backColor, Vec2(-0.691314, 0.328133), Vec2(-0.746006, 0.375002), Vec3(0.571, 0.569, 0.927), Vec3(0.635, 0.624, 0.969));
		AddLine(points, index, frontColor, backColor, Vec2(-0.713022, 0.223982), Vec2(-0.699215, 0.078105), Vec3(0.443, 0.404, 0.831), Vec3(0.490, 0.471, 0.855), Vec3(0.486, 0.455, 0.871), Vec3(0.576, 0.573, 0.929));
		AddLine(points, index, frontColor, backColor, Vec2(-0.687633, 0.328193), Vec2(-0.713148, 0.224068), Vec3(0.502, 0.473, 0.884), Vec3(0.590, 0.586, 0.943));
		AddLine(points, index, frontColor, backColor, Vec2(-0.962077, -0.021061), Vec2(-0.984470, -0.062241), Vec3(0.567, 0.553, 0.914), Vec3(0.506, 0.502, 0.843));
		AddLine(points, index, frontColor, backColor, Vec2(-0.929730, 0.012170), Vec2(-0.962095, -0.021151), Vec3(0.563, 0.543, 0.914), Vec3(0.508, 0.514, 0.829));
		AddLine(points, index, frontColor, backColor, Vec2(-0.842855, 0.056023), Vec2(-0.929597, 0.011852), Vec3(0.555, 0.527, 0.914), Vec3(0.506, 0.522, 0.804));
		AddLine(points, index, frontColor, backColor, Vec2(-0.740058, 0.072302), Vec2(-0.842912, 0.056291), Vec3(0.549, 0.518, 0.914), Vec3(0.502, 0.522, 0.788), Vec3(0.537, 0.498, 0.914), Vec3(0.431, 0.412, 0.788));
		AddLine(points, index, frontColor, backColor, Vec2(-0.648617, 0.066357), Vec2(-0.739962, 0.072359), Vec3(0.531, 0.486, 0.912), Vec3(0.406, 0.369, 0.786));
		AddLine(points, index, frontColor, backColor, Vec2(-0.599562, 0.052212), Vec2(-0.648693, 0.066421), Vec3(0.522, 0.465, 0.912), Vec3(0.390, 0.339, 0.776));
		AddLine(points, index, frontColor, backColor, Vec2(-0.523674, -0.011767), Vec2(-0.599782, 0.051913), Vec3(0.510, 0.443, 0.912), Vec3(0.416, 0.371, 0.761));
		AddLine(points, index, frontColor, backColor, Vec2(-0.216909, 0.467215), Vec2(-0.261703, 0.386747), Vec3(0.943, 0.525, 0.555), Vec3(0.851, 0.796, 0.924));
		AddLine(points, index, frontColor, backColor, Vec2(-0.140684, 0.554868), Vec2(-0.216981, 0.467356), Vec3(0.898, 0.506, 0.527), Vec3(0.851, 0.786, 0.865));
		AddLine(points, index, frontColor, backColor, Vec2(-0.294609, 0.328762), Vec2(-0.320297, 0.253955), Vec3(0.945, 0.480, 0.496), Vec3(0.655, 0.545, 0.908));
		AddLine(points, index, frontColor, backColor, Vec2(-0.261515, 0.386675), Vec2(-0.294411, 0.328660), Vec3(0.953, 0.482, 0.498), Vec3(0.678, 0.576, 0.945), Vec3(0.953, 0.482, 0.494), Vec3(0.776, 0.694, 0.961));
		AddLine(points, index, frontColor, backColor, Vec2(-0.320312, 0.152344), Vec2(-0.320312, 0.250000), Vec3(0.227, 0.159, 0.249), Vec3(0.892, 0.455, 0.476));
		AddLine(points, index, frontColor, backColor, Vec2(-0.285102, 0.093592), Vec2(-0.320160, 0.152217), Vec3(0.194, 0.129, 0.220), Vec3(0.878, 0.441, 0.463));
		AddLine(points, index, frontColor, backColor, Vec2(-0.249821, 0.058499), Vec2(-0.285083, 0.093620), Vec3(0.206, 0.137, 0.220), Vec3(0.861, 0.422, 0.445));
		AddLine(points, index, frontColor, backColor, Vec2(-0.370886, 0.200884), Vec2(-0.296881, 0.058849), Vec3(0.210, 0.208, 0.296), Vec3(0.506, 0.441, 0.829));
		AddLine(points, index, frontColor, backColor, Vec2(-0.371080, 0.218669), Vec2(-0.370863, 0.200571), Vec3(0.216, 0.208, 0.275), Vec3(0.539, 0.473, 0.853));
		AddLine(points, index, frontColor, backColor, Vec2(-0.320338, 0.250030), Vec2(-0.371125, 0.218779), Vec3(0.214, 0.182, 0.245), Vec3(0.573, 0.504, 0.873));
		AddLine(points, index, frontColor, backColor, Vec2(-0.318397, 0.038871), Vec2(-0.296987, 0.058686), Vec3(0.408, 0.345, 0.751), Vec3(0.235, 0.243, 0.371));
		AddLine(points, index, frontColor, backColor, Vec2(-0.335799, 0.000126), Vec2(-0.318147, 0.038906), Vec3(0.394, 0.337, 0.686), Vec3(0.225, 0.229, 0.355));
		AddLine(points, index, frontColor, backColor, Vec2(-0.457045, 0.015479), Vec2(-0.343771, 0.003763), Vec3(0.404, 0.341, 0.757), Vec3(0.714, 0.702, 0.792), Vec3(0.427, 0.369, 0.796), Vec3(0.478, 0.443, 0.510));
		AddLine(points, index, frontColor, backColor, Vec2(-0.524601, -0.011832), Vec2(-0.457157, 0.015632), Vec3(0.427, 0.369, 0.796), Vec3(0.478, 0.443, 0.510), Vec3(0.475, 0.424, 0.792), Vec3(0.627, 0.596, 0.741));
		AddLine(points, index, frontColor, backColor, Vec2(-0.567845, -0.048929), Vec2(-0.524475, -0.011939), Vec3(0.475, 0.424, 0.792), Vec3(0.627, 0.596, 0.741), Vec3(0.545, 0.490, 0.871), Vec3(0.522, 0.490, 0.647));
		AddLine(points, index, frontColor, backColor, Vec2(-0.687512, -0.202819), Vec2(-0.567697, -0.048786), Vec3(0.545, 0.490, 0.871), Vec3(0.522, 0.490, 0.647), Vec3(0.749, 0.706, 0.980), Vec3(0.702, 0.651, 0.710));
		AddLine(points, index, frontColor, backColor, Vec2(-0.738370, -0.246033), Vec2(-0.687782, -0.202852), Vec3(0.749, 0.706, 0.980), Vec3(0.702, 0.651, 0.710), Vec3(0.690, 0.651, 0.929), Vec3(0.259, 0.235, 0.337));
		AddLine(points, index, frontColor, backColor, Vec2(-0.819820, -0.267858), Vec2(-0.738322, -0.246131), Vec3(0.690, 0.651, 0.929), Vec3(0.259, 0.235, 0.337), Vec3(0.631, 0.604, 0.933), Vec3(0.380, 0.333, 0.416));
		AddLine(points, index, frontColor, backColor, Vec2(-0.925652, -0.360953), Vec2(-0.819489, -0.267857), Vec3(0.631, 0.604, 0.933), Vec3(0.380, 0.333, 0.416), Vec3(0.525, 0.502, 0.808), Vec3(0.243, 0.188, 0.333));
		AddLine(points, index, frontColor, backColor, Vec2(-0.924077, -0.382938), Vec2(-0.926012, -0.360824), Vec3(0.525, 0.502, 0.808), Vec3(0.243, 0.188, 0.333), Vec3(0.506, 0.478, 0.733), Vec3(0.541, 0.486, 0.631));
		AddLine(points, index, frontColor, backColor, Vec2(-0.908709, -0.396678), Vec2(-0.923914, -0.383129), Vec3(0.502, 0.473, 0.714), Vec3(0.524, 0.476, 0.637));
		AddLine(points, index, frontColor, backColor, Vec2(-0.877801, -0.327818), Vec2(-0.908834, -0.396482), Vec3(0.498, 0.467, 0.694), Vec3(0.506, 0.467, 0.643), Vec3(0.584, 0.565, 0.894), Vec3(0.490, 0.439, 0.659));
		AddLine(points, index, frontColor, backColor, Vec2(-0.850665, -0.306523), Vec2(-0.877737, -0.328046), Vec3(0.584, 0.565, 0.894), Vec3(0.490, 0.439, 0.659), Vec3(0.553, 0.510, 0.765), Vec3(0.329, 0.278, 0.439));
		AddLine(points, index, frontColor, backColor, Vec2(-0.842943, -0.336355), Vec2(-0.850644, -0.306423), Vec3(0.553, 0.510, 0.765), Vec3(0.329, 0.278, 0.439), Vec3(0.518, 0.478, 0.761), Vec3(0.537, 0.494, 0.643));
		AddLine(points, index, frontColor, backColor, Vec2(-0.646520, -0.253453), Vec2(-0.842985, -0.336477), Vec3(0.518, 0.478, 0.761), Vec3(0.537, 0.494, 0.643), Vec3(0.671, 0.620, 0.957), Vec3(0.467, 0.392, 0.565));
		AddLine(points, index, frontColor, backColor, Vec2(-0.613162, -0.227037), Vec2(-0.646645, -0.253104), Vec3(0.651, 0.602, 0.945), Vec3(0.469, 0.398, 0.555));
		AddLine(points, index, frontColor, backColor, Vec2(-0.561901, -0.162148), Vec2(-0.613081, -0.227201), Vec3(0.594, 0.549, 0.910), Vec3(0.476, 0.429, 0.539));
		AddLine(points, index, frontColor, backColor, Vec2(-0.500240, -0.050707), Vec2(-0.562155, -0.162105), Vec3(0.557, 0.514, 0.886), Vec3(0.482, 0.455, 0.533), Vec3(0.443, 0.408, 0.812), Vec3(0.286, 0.267, 0.369));
		AddLine(points, index, frontColor, backColor, Vec2(-0.464788, -0.030266), Vec2(-0.500093, -0.050719), Vec3(0.443, 0.408, 0.812), Vec3(0.286, 0.267, 0.369), Vec3(0.408, 0.376, 0.780), Vec3(0.157, 0.114, 0.267));
		AddLine(points, index, frontColor, backColor, Vec2(-0.439427, -0.045861), Vec2(-0.464616, -0.030239), Vec3(0.408, 0.376, 0.780), Vec3(0.157, 0.114, 0.267), Vec3(0.455, 0.427, 0.784), Vec3(0.416, 0.353, 0.435));
		AddLine(points, index, frontColor, backColor, Vec2(-0.421871, -0.081990), Vec2(-0.439501, -0.045912), Vec3(0.443, 0.418, 0.773), Vec3(0.404, 0.369, 0.437));
		AddLine(points, index, frontColor, backColor, Vec2(-0.386711, -0.105557), Vec2(-0.421868, -0.082119), Vec3(0.431, 0.408, 0.761), Vec3(0.392, 0.384, 0.439), Vec3(0.357, 0.349, 0.655), Vec3(0.376, 0.427, 0.447));
		AddLine(points, index, frontColor, backColor, Vec2(-0.339923, 0.003866), Vec2(-0.386802, -0.101587), Vec3(0.200, 0.196, 0.267), Vec3(0.345, 0.333, 0.365));
		AddLine(points, index, frontColor, backColor, Vec2(-0.407061, -0.201258), Vec2(-0.386665, -0.105627), Vec3(0.361, 0.341, 0.624), Vec3(0.792, 0.812, 0.751));
		AddLine(points, index, frontColor, backColor, Vec2(-0.403323, -0.268221), Vec2(-0.406911, -0.201492), Vec3(0.363, 0.329, 0.606), Vec3(0.771, 0.769, 0.724));
		AddLine(points, index, frontColor, backColor, Vec2(-0.390770, -0.273672), Vec2(-0.403673, -0.268054), Vec3(0.384, 0.333, 0.643), Vec3(0.771, 0.749, 0.704));
		AddLine(points, index, frontColor, backColor, Vec2(-0.345881, -0.060745), Vec2(-0.386656, -0.101656), Vec3(0.776, 0.835, 0.784), Vec3(0.361, 0.400, 0.427), Vec3(0.792, 0.839, 0.780), Vec3(0.145, 0.176, 0.227));
		AddLine(points, index, frontColor, backColor, Vec2(-0.328983, -0.056141), Vec2(-0.345818, -0.060743), Vec3(0.792, 0.839, 0.780), Vec3(0.145, 0.176, 0.227), Vec3(0.808, 0.847, 0.780), Vec3(0.220, 0.251, 0.310));
		AddLine(points, index, frontColor, backColor, Vec2(-0.252749, -0.126588), Vec2(-0.329397, -0.055845), Vec3(0.837, 0.859, 0.775), Vec3(0.227, 0.247, 0.306));
		AddLine(points, index, frontColor, backColor, Vec2(-0.230481, -0.160125), Vec2(-0.252484, -0.126757), Vec3(0.867, 0.871, 0.769), Vec3(0.235, 0.243, 0.302), Vec3(0.859, 0.851, 0.749), Vec3(0.161, 0.153, 0.204));
		AddLine(points, index, frontColor, backColor, Vec2(-0.226341, -0.219167), Vec2(-0.230372, -0.160222), Vec3(0.882, 0.869, 0.757), Vec3(0.151, 0.133, 0.180));
		AddLine(points, index, frontColor, backColor, Vec2(-0.277439, -0.343654), Vec2(-0.226534, -0.219015), Vec3(0.906, 0.886, 0.765), Vec3(0.141, 0.114, 0.157), Vec3(0.929, 0.898, 0.761), Vec3(0.251, 0.212, 0.239));
		AddLine(points, index, frontColor, backColor, Vec2(-0.331270, -0.274257), Vec2(-0.390414, -0.273335), Vec3(0.076, 0.061, 0.096), Vec3(0.808, 0.780, 0.714));
		AddLine(points, index, frontColor, backColor, Vec2(-0.307108, -0.285553), Vec2(-0.331497, -0.274239), Vec3(0.075, 0.059, 0.094), Vec3(0.829, 0.806, 0.718));
		AddLine(points, index, frontColor, backColor, Vec2(-0.285269, -0.316297), Vec2(-0.307202, -0.285572), Vec3(0.075, 0.059, 0.094), Vec3(0.835, 0.820, 0.722));
		AddLine(points, index, frontColor, backColor, Vec2(-0.277101, -0.348165), Vec2(-0.284918, -0.316904), Vec3(0.075, 0.059, 0.094), Vec3(0.831, 0.820, 0.722), Vec3(0.086, 0.063, 0.102), Vec3(0.620, 0.584, 0.522));
		AddLine(points, index, frontColor, backColor, Vec2(0.191818, -0.655109), Vec2(0.210666, -0.574643), Vec3(0.137, 0.122, 0.129), Vec3(0.729, 0.608, 0.541), Vec3(0.145, 0.133, 0.133), Vec3(0.804, 0.737, 0.616));
		AddLine(points, index, frontColor, backColor, Vec2(0.177802, -0.664657), Vec2(0.191779, -0.655002), Vec3(0.145, 0.133, 0.133), Vec3(0.804, 0.737, 0.616), Vec3(0.063, 0.059, 0.075), Vec3(0.796, 0.753, 0.620));
		AddLine(points, index, frontColor, backColor, Vec2(0.168515, -0.738957), Vec2(0.178126, -0.664256), Vec3(0.096, 0.092, 0.100), Vec3(0.812, 0.759, 0.625));
		AddLine(points, index, frontColor, backColor, Vec2(0.155592, -0.752345), Vec2(0.168584, -0.738793), Vec3(0.104, 0.100, 0.102), Vec3(0.824, 0.757, 0.627));
		AddLine(points, index, frontColor, backColor, Vec2(0.132513, -0.757861), Vec2(0.155207, -0.752647), Vec3(0.073, 0.069, 0.073), Vec3(0.820, 0.747, 0.624));
		AddLine(points, index, frontColor, backColor, Vec2(0.086261, -0.759725), Vec2(-0.023197, -0.695559), Vec3(0.157, 0.141, 0.184), Vec3(0.063, 0.055, 0.067), Vec3(0.286, 0.275, 0.325), Vec3(0.055, 0.043, 0.055));
		AddLine(points, index, frontColor, backColor, Vec2(0.132843, -0.757744), Vec2(0.086059, -0.759407), Vec3(0.286, 0.275, 0.325), Vec3(0.055, 0.043, 0.055), Vec3(0.761, 0.722, 0.522), Vec3(0.059, 0.043, 0.059));
		AddLine(points, index, frontColor, backColor, Vec2(-0.008536, -0.523957), Vec2(-0.113095, -0.546922), Vec3(0.761, 0.722, 0.522), Vec3(0.051, 0.033, 0.051));
		AddLine(points, index, frontColor, backColor, Vec2(0.028417, -0.535309), Vec2(-0.008810, -0.523762), Vec3(0.792, 0.767, 0.563), Vec3(0.055, 0.041, 0.057));
		AddLine(points, index, frontColor, backColor, Vec2(0.049655, -0.559962), Vec2(0.028517, -0.535386), Vec3(0.798, 0.782, 0.580), Vec3(0.076, 0.065, 0.073));
		AddLine(points, index, frontColor, backColor, Vec2(0.071714, -0.659300), Vec2(0.049649, -0.559687), Vec3(0.771, 0.751, 0.557), Vec3(0.082, 0.073, 0.080));
		AddLine(points, index, frontColor, backColor, Vec2(0.062107, -0.673453), Vec2(0.072068, -0.659420), Vec3(0.731, 0.704, 0.520), Vec3(0.088, 0.080, 0.088));
		AddLine(points, index, frontColor, backColor, Vec2(-0.019664, -0.695497), Vec2(0.061596, -0.673865), Vec3(0.722, 0.694, 0.514), Vec3(0.090, 0.082, 0.090), Vec3(0.722, 0.690, 0.502), Vec3(0.220, 0.216, 0.180));
		AddLine(points, index, frontColor, backColor, Vec2(-0.103143, -0.577233), Vec2(-0.113704, -0.546335), Vec3(0.078, 0.063, 0.094), Vec3(0.669, 0.586, 0.406));
		AddLine(points, index, frontColor, backColor, Vec2(-0.128872, -0.656514), Vec2(-0.102680, -0.578043), Vec3(0.078, 0.063, 0.094), Vec3(0.643, 0.573, 0.396), Vec3(0.078, 0.063, 0.094), Vec3(0.545, 0.506, 0.349));
		AddLine(points, index, frontColor, backColor, Vec2(-0.110341, -0.684318), Vec2(-0.132708, -0.656415), Vec3(0.090, 0.076, 0.129), Vec3(0.712, 0.680, 0.494));
		AddLine(points, index, frontColor, backColor, Vec2(-0.065097, -0.698489), Vec2(-0.110606, -0.684138), Vec3(0.106, 0.094, 0.133), Vec3(0.712, 0.680, 0.494));
		AddLine(points, index, frontColor, backColor, Vec2(-0.023516, -0.695404), Vec2(-0.065017, -0.698593), Vec3(0.124, 0.112, 0.137), Vec3(0.718, 0.686, 0.498));
		AddLine(points, index, frontColor, backColor, Vec2(-0.356409, -0.396602), Vec2(-0.363344, -0.417840), Vec3(0.667, 0.684, 0.514), Vec3(0.086, 0.073, 0.102));
		AddLine(points, index, frontColor, backColor, Vec2(-0.329514, -0.372734), Vec2(-0.356301, -0.396902), Vec3(0.678, 0.694, 0.525), Vec3(0.098, 0.086, 0.110), Vec3(0.796, 0.792, 0.624), Vec3(0.075, 0.059, 0.094));
		AddLine(points, index, frontColor, backColor, Vec2(-0.304495, -0.384405), Vec2(-0.329372, -0.372760), Vec3(0.794, 0.786, 0.625), Vec3(0.073, 0.055, 0.094));
		AddLine(points, index, frontColor, backColor, Vec2(-0.304686, -0.445287), Vec2(-0.304686, -0.384176), Vec3(0.792, 0.780, 0.627), Vec3(0.071, 0.051, 0.094), Vec3(0.678, 0.667, 0.541), Vec3(0.071, 0.047, 0.094));
		AddLine(points, index, frontColor, backColor, Vec2(-0.329703, -0.471680), Vec2(-0.304762, -0.445327), Vec3(0.678, 0.667, 0.541), Vec3(0.071, 0.047, 0.094), Vec3(0.533, 0.522, 0.427), Vec3(0.071, 0.051, 0.094));
		AddLine(points, index, frontColor, backColor, Vec2(-0.352235, -0.461623), Vec2(-0.329757, -0.471344), Vec3(0.504, 0.494, 0.406), Vec3(0.071, 0.051, 0.094));
		AddLine(points, index, frontColor, backColor, Vec2(-0.363307, -0.418071), Vec2(-0.352136, -0.462043), Vec3(0.475, 0.467, 0.384), Vec3(0.071, 0.051, 0.094), Vec3(0.561, 0.565, 0.439), Vec3(0.129, 0.118, 0.137));
		AddLine(points, index, frontColor, backColor, Vec2(-0.276110, -0.521710), Vec2(-0.289045, -0.550497), Vec3(0.641, 0.637, 0.478), Vec3(0.035, 0.033, 0.063));
		AddLine(points, index, frontColor, backColor, Vec2(-0.258075, -0.516729), Vec2(-0.275890, -0.521970), Vec3(0.684, 0.682, 0.514), Vec3(0.047, 0.045, 0.075));
		AddLine(points, index, frontColor, backColor, Vec2(-0.206378, -0.541564), Vec2(-0.258683, -0.516738), Vec3(0.706, 0.706, 0.529), Vec3(0.047, 0.047, 0.078), Vec3(0.792, 0.788, 0.600), Vec3(0.051, 0.055, 0.063));
		AddLine(points, index, frontColor, backColor, Vec2(-0.187895, -0.563812), Vec2(-0.206382, -0.541493), Vec3(0.757, 0.753, 0.576), Vec3(0.063, 0.057, 0.078));
		AddLine(points, index, frontColor, backColor, Vec2(-0.189739, -0.573083), Vec2(-0.187133, -0.563528), Vec3(0.735, 0.731, 0.561), Vec3(0.075, 0.059, 0.094));
		AddLine(points, index, frontColor, backColor, Vec2(-0.251815, -0.589992), Vec2(-0.189995, -0.573544), Vec3(0.749, 0.745, 0.569), Vec3(0.075, 0.059, 0.094), Vec3(0.498, 0.502, 0.345), Vec3(0.075, 0.063, 0.098));
		AddLine(points, index, frontColor, backColor, Vec2(-0.272446, -0.579505), Vec2(-0.251715, -0.590033), Vec3(0.514, 0.518, 0.363), Vec3(0.075, 0.063, 0.098));
		AddLine(points, index, frontColor, backColor, Vec2(-0.289115, -0.550792), Vec2(-0.272717, -0.579388), Vec3(0.549, 0.551, 0.398), Vec3(0.086, 0.075, 0.106));
		AddLine(points, index, frontColor, backColor, Vec2(-0.192736, -0.469520), Vec2(-0.277483, -0.347082), Vec3(0.108, 0.076, 0.120), Vec3(0.065, 0.057, 0.090));
		AddLine(points, index, frontColor, backColor, Vec2(-0.113128, -0.547007), Vec2(-0.192449, -0.470244), Vec3(0.080, 0.045, 0.080), Vec3(0.063, 0.047, 0.073));
		AddLine(points, index, frontColor, backColor, Vec2(-0.399689, -0.363777), Vec2(-0.390465, -0.273580), Vec3(0.355, 0.292, 0.675), Vec3(0.088, 0.075, 0.112));
		AddLine(points, index, frontColor, backColor, Vec2(-0.414173, -0.390763), Vec2(-0.399958, -0.363772), Vec3(0.347, 0.288, 0.651), Vec3(0.090, 0.075, 0.110));
		AddLine(points, index, frontColor, backColor, Vec2(0.402104, -0.164371), Vec2(0.429754, -0.097640), Vec3(0.237, 0.182, 0.180), Vec3(0.784, 0.296, 0.143));
		AddLine(points, index, frontColor, backColor, Vec2(0.366996, -0.206976), Vec2(0.401848, -0.164337), Vec3(0.210, 0.145, 0.145), Vec3(0.778, 0.304, 0.147));
		AddLine(points, index, frontColor, backColor, Vec2(0.296913, -0.238165), Vec2(0.367225, -0.206912), Vec3(0.194, 0.118, 0.120), Vec3(0.743, 0.304, 0.149));
		AddLine(points, index, frontColor, backColor, Vec2(0.167900, -0.203204), Vec2(0.113213, -0.128975), Vec3(0.706, 0.690, 0.678), Vec3(0.145, 0.110, 0.157));
		AddLine(points, index, frontColor, backColor, Vec2(0.454448, -0.085567), Vec2(0.429490, -0.097974), Vec3(0.898, 0.478, 0.357), Vec3(0.945, 0.643, 0.580));
		AddLine(points, index, frontColor, backColor, Vec2(0.501070, -0.040927), Vec2(0.454701, -0.085293), Vec3(0.898, 0.475, 0.349), Vec3(0.941, 0.631, 0.561), Vec3(0.902, 0.439, 0.278), Vec3(0.910, 0.525, 0.416));
		AddLine(points, index, frontColor, backColor, Vec2(0.536751, 0.019080), Vec2(0.501035, -0.040870), Vec3(0.904, 0.425, 0.247), Vec3(0.902, 0.502, 0.384));
		AddLine(points, index, frontColor, backColor, Vec2(0.554739, 0.081950), Vec2(0.536789, 0.018991), Vec3(0.906, 0.412, 0.216), Vec3(0.900, 0.508, 0.390));
		AddLine(points, index, frontColor, backColor, Vec2(0.535861, 0.166934), Vec2(0.554677, 0.082064), Vec3(0.906, 0.412, 0.216), Vec3(0.906, 0.537, 0.427), Vec3(0.906, 0.416, 0.224), Vec3(0.925, 0.647, 0.557));
		AddLine(points, index, frontColor, backColor, Vec2(0.419826, 0.338301), Vec2(0.535615, 0.166647), Vec3(0.906, 0.414, 0.220), Vec3(0.920, 0.614, 0.524));
		AddLine(points, index, frontColor, backColor, Vec2(0.406078, 0.390921), Vec2(0.419883, 0.338831), Vec3(0.906, 0.412, 0.216), Vec3(0.910, 0.565, 0.473));
		AddLine(points, index, frontColor, backColor, Vec2(0.146477, 0.567224), Vec2(0.031189, 0.523314), Vec3(0.863, 0.475, 0.404), Vec3(0.963, 0.447, 0.278));
		AddLine(points, index, frontColor, backColor, Vec2(0.238636, 0.574102), Vec2(0.146891, 0.567233), Vec3(0.869, 0.484, 0.425), Vec3(0.957, 0.427, 0.241));
		AddLine(points, index, frontColor, backColor, Vec2(0.276654, 0.562718), Vec2(0.238421, 0.574198), Vec3(0.876, 0.496, 0.445), Vec3(0.949, 0.400, 0.192));
		AddLine(points, index, frontColor, backColor, Vec2(0.328054, 0.527362), Vec2(0.276439, 0.562762), Vec3(0.880, 0.508, 0.463), Vec3(0.939, 0.382, 0.165));
		AddLine(points, index, frontColor, backColor, Vec2(0.407058, 0.213498), Vec2(0.371072, 0.195345), Vec3(0.988, 0.973, 0.969), Vec3(0.992, 0.855, 0.831));
		AddLine(points, index, frontColor, backColor, Vec2(0.458303, 0.213407), Vec2(0.407282, 0.213566), Vec3(0.988, 0.973, 0.969), Vec3(0.992, 0.855, 0.831));
		AddLine(points, index, frontColor, backColor, Vec2(0.481278, 0.196816), Vec2(0.457990, 0.213665), Vec3(0.988, 0.973, 0.967), Vec3(0.992, 0.855, 0.829));
		AddLine(points, index, frontColor, backColor, Vec2(0.499009, 0.163011), Vec2(0.481392, 0.196333), Vec3(0.988, 0.973, 0.965), Vec3(0.992, 0.855, 0.827));
		AddLine(points, index, frontColor, backColor, Vec2(0.503604, 0.090461), Vec2(0.499073, 0.163192), Vec3(0.988, 0.971, 0.965), Vec3(0.990, 0.855, 0.827));
		AddLine(points, index, frontColor, backColor, Vec2(0.489446, 0.076448), Vec2(0.503554, 0.090238), Vec3(0.988, 0.969, 0.963), Vec3(0.986, 0.847, 0.816));
		AddLine(points, index, frontColor, backColor, Vec2(0.443782, 0.062339), Vec2(0.489220, 0.076528), Vec3(0.986, 0.969, 0.961), Vec3(0.986, 0.845, 0.814));
		AddLine(points, index, frontColor, backColor, Vec2(0.378683, 0.066466), Vec2(0.443770, 0.062469), Vec3(0.984, 0.969, 0.959), Vec3(0.988, 0.851, 0.822));
		AddLine(points, index, frontColor, backColor, Vec2(0.339578, 0.097522), Vec2(0.378706, 0.066500), Vec3(0.984, 0.967, 0.957), Vec3(0.986, 0.851, 0.820));
		AddLine(points, index, frontColor, backColor, Vec2(0.329906, 0.140698), Vec2(0.339788, 0.097651), Vec3(0.984, 0.965, 0.955), Vec3(0.984, 0.851, 0.818));
		AddLine(points, index, frontColor, backColor, Vec2(0.349815, 0.180720), Vec2(0.329907, 0.140407), Vec3(0.984, 0.965, 0.953), Vec3(0.982, 0.849, 0.814));
		AddLine(points, index, frontColor, backColor, Vec2(0.371027, 0.195457), Vec2(0.349730, 0.180934), Vec3(0.984, 0.965, 0.953), Vec3(0.982, 0.849, 0.814));
		AddLine(points, index, frontColor, backColor, Vec2(-0.117425, 0.341220), Vec2(-0.074257, 0.417986), Vec3(0.847, 0.318, 0.216), Vec3(0.794, 0.425, 0.357));
		AddLine(points, index, frontColor, backColor, Vec2(-0.131058, 0.286069), Vec2(-0.117280, 0.341245), Vec3(0.847, 0.318, 0.216), Vec3(0.814, 0.429, 0.355));
		AddLine(points, index, frontColor, backColor, Vec2(-0.120979, 0.239269), Vec2(-0.131229, 0.286301), Vec3(0.847, 0.318, 0.216), Vec3(0.831, 0.435, 0.355));
		AddLine(points, index, frontColor, backColor, Vec2(-0.094915, 0.195662), Vec2(-0.120748, 0.238869), Vec3(0.853, 0.308, 0.194), Vec3(0.847, 0.439, 0.355));
		AddLine(points, index, frontColor, backColor, Vec2(-0.058454, 0.163892), Vec2(-0.094941, 0.195621), Vec3(0.861, 0.296, 0.171), Vec3(0.861, 0.439, 0.351));
		AddLine(points, index, frontColor, backColor, Vec2(0.096154, 0.101755), Vec2(-0.058568, 0.164000), Vec3(0.855, 0.304, 0.188), Vec3(0.884, 0.443, 0.349));
		AddLine(points, index, frontColor, backColor, Vec2(0.148627, 0.066296), Vec2(0.096309, 0.101713), Vec3(0.845, 0.316, 0.214), Vec3(0.906, 0.447, 0.349));
		AddLine(points, index, frontColor, backColor, Vec2(0.238337, -0.218583), Vec2(0.296931, -0.238114), Vec3(0.188, 0.059, 0.055), Vec3(0.941, 0.714, 0.384));
		AddLine(points, index, frontColor, backColor, Vec2(0.356732, 0.495659), Vec2(0.328217, 0.527093), Vec3(0.475, 0.510, 0.604), Vec3(0.418, 0.445, 0.533));
		AddLine(points, index, frontColor, backColor, Vec2(0.406416, 0.390930), Vec2(0.356795, 0.496206), Vec3(0.475, 0.510, 0.604), Vec3(0.398, 0.414, 0.496));
		AddLine(points, index, frontColor, backColor, Vec2(0.351847, 0.563942), Vec2(0.328272, 0.527247), Vec3(0.439, 0.475, 0.569), Vec3(0.725, 0.459, 0.408), Vec3(0.443, 0.475, 0.569), Vec3(0.882, 0.420, 0.259));
		AddLine(points, index, frontColor, backColor, Vec2(0.386954, 0.567493), Vec2(0.352309, 0.564131), Vec3(0.431, 0.453, 0.537), Vec3(0.884, 0.416, 0.247));
		AddLine(points, index, frontColor, backColor, Vec2(0.434360, 0.555848), Vec2(0.386390, 0.567397), Vec3(0.420, 0.431, 0.506), Vec3(0.886, 0.412, 0.235), Vec3(0.361, 0.341, 0.396), Vec3(0.882, 0.416, 0.247));
		AddLine(points, index, frontColor, backColor, Vec2(0.481493, 0.527439), Vec2(0.434614, 0.555661), Vec3(0.376, 0.353, 0.406), Vec3(0.882, 0.416, 0.247));
		AddLine(points, index, frontColor, backColor, Vec2(0.556668, 0.460958), Vec2(0.481472, 0.527483), Vec3(0.392, 0.365, 0.416), Vec3(0.882, 0.416, 0.247), Vec3(0.522, 0.502, 0.557), Vec3(0.882, 0.412, 0.247));
		AddLine(points, index, frontColor, backColor, Vec2(0.568077, 0.433950), Vec2(0.556441, 0.461051), Vec3(0.524, 0.506, 0.563), Vec3(0.880, 0.414, 0.247));
		AddLine(points, index, frontColor, backColor, Vec2(0.550912, 0.394412), Vec2(0.568131, 0.433870), Vec3(0.494, 0.480, 0.541), Vec3(0.880, 0.416, 0.247));
		AddLine(points, index, frontColor, backColor, Vec2(0.490768, 0.382274), Vec2(0.550937, 0.394469), Vec3(0.422, 0.425, 0.496), Vec3(0.882, 0.416, 0.247));
		AddLine(points, index, frontColor, backColor, Vec2(0.406039, 0.390810), Vec2(0.490402, 0.382519), Vec3(0.375, 0.382, 0.457), Vec3(0.882, 0.418, 0.253));
		AddLine(points, index, frontColor, backColor, Vec2(0.330744, 0.478846), Vec2(0.328238, 0.527133), Vec3(0.908, 0.586, 0.498), Vec3(0.475, 0.510, 0.604));
		AddLine(points, index, frontColor, backColor, Vec2(0.365538, 0.422127), Vec2(0.330688, 0.479010), Vec3(0.910, 0.588, 0.498), Vec3(0.475, 0.510, 0.604));
		AddLine(points, index, frontColor, backColor, Vec2(0.406309, 0.390576), Vec2(0.365537, 0.422123), Vec3(0.910, 0.588, 0.498), Vec3(0.475, 0.502, 0.592));
		AddLine(points, index, frontColor, backColor, Vec2(0.037170, 0.502175), Vec2(0.031325, 0.523294), Vec3(0.357, 0.220, 0.255), Vec3(0.725, 0.404, 0.349), Vec3(0.357, 0.220, 0.255), Vec3(0.914, 0.506, 0.408));
		AddLine(points, index, frontColor, backColor, Vec2(0.009112, 0.456598), Vec2(0.036991, 0.502416), Vec3(0.357, 0.220, 0.255), Vec3(0.900, 0.484, 0.404));
		AddLine(points, index, frontColor, backColor, Vec2(-0.014264, 0.437994), Vec2(0.009264, 0.456398), Vec3(0.357, 0.220, 0.255), Vec3(0.882, 0.457, 0.398));
		AddLine(points, index, frontColor, backColor, Vec2(-0.074384, 0.418060), Vec2(-0.014479, 0.438192), Vec3(0.357, 0.220, 0.255), Vec3(0.873, 0.443, 0.394));
		AddLine(points, index, frontColor, backColor, Vec2(-0.034865, 0.469095), Vec2(-0.074326, 0.418051), Vec3(0.357, 0.220, 0.255), Vec3(0.325, 0.153, 0.310));
		AddLine(points, index, frontColor, backColor, Vec2(0.031160, 0.519567), Vec2(-0.034850, 0.469051), Vec3(0.357, 0.220, 0.255), Vec3(0.312, 0.147, 0.269));
		AddLine(points, index, frontColor, backColor, Vec2(-0.476788, -0.270699), Vec2(-0.413803, -0.390958), Vec3(0.400, 0.333, 0.724), Vec3(0.529, 0.406, 0.447));
		AddLine(points, index, frontColor, backColor, Vec2(-0.515771, -0.226315), Vec2(-0.477190, -0.270110), Vec3(0.439, 0.367, 0.784), Vec3(0.571, 0.437, 0.476));
		AddLine(points, index, frontColor, backColor, Vec2(-0.548925, -0.216192), Vec2(-0.515512, -0.226748), Vec3(0.475, 0.408, 0.808), Vec3(0.588, 0.451, 0.490), Vec3(0.580, 0.529, 0.878), Vec3(0.404, 0.282, 0.416));
		AddLine(points, index, frontColor, backColor, Vec2(-0.563920, -0.221379), Vec2(-0.548931, -0.216115), Vec3(0.580, 0.529, 0.878), Vec3(0.404, 0.282, 0.416), Vec3(0.671, 0.635, 0.937), Vec3(0.533, 0.455, 0.702));
		AddLine(points, index, frontColor, backColor, Vec2(-0.487346, -0.300005), Vec2(-0.564121, -0.221089), Vec3(0.671, 0.635, 0.937), Vec3(0.533, 0.455, 0.702), Vec3(0.502, 0.459, 0.749), Vec3(0.522, 0.392, 0.561));
		AddLine(points, index, frontColor, backColor, Vec2(-0.456709, -0.351242), Vec2(-0.487322, -0.300058), Vec3(0.502, 0.459, 0.749), Vec3(0.522, 0.392, 0.561), Vec3(0.396, 0.349, 0.639), Vec3(0.412, 0.290, 0.471));
		AddLine(points, index, frontColor, backColor, Vec2(-0.437438, -0.426111), Vec2(-0.456570, -0.351699), Vec3(0.396, 0.349, 0.639), Vec3(0.412, 0.290, 0.471), Vec3(0.294, 0.243, 0.537), Vec3(0.553, 0.475, 0.596));
		AddLine(points, index, frontColor, backColor, Vec2(-0.426103, -0.437341), Vec2(-0.437819, -0.425625), Vec3(0.294, 0.243, 0.537), Vec3(0.553, 0.475, 0.596), Vec3(0.380, 0.345, 0.600), Vec3(0.659, 0.600, 0.761));
		AddLine(points, index, frontColor, backColor, Vec2(-0.137942, -0.735671), Vec2(-0.234693, -0.652174), Vec3(0.390, 0.329, 0.331), Vec3(0.078, 0.075, 0.145));
		AddLine(points, index, frontColor, backColor, Vec2(-0.101453, -0.787596), Vec2(-0.137531, -0.736006), Vec3(0.376, 0.318, 0.302), Vec3(0.067, 0.067, 0.141), Vec3(0.729, 0.698, 0.671), Vec3(0.051, 0.055, 0.137));
		AddLine(points, index, frontColor, backColor, Vec2(-0.093467, -0.816814), Vec2(-0.101262, -0.787835), Vec3(0.729, 0.698, 0.671), Vec3(0.051, 0.055, 0.137), Vec3(0.592, 0.549, 0.522), Vec3(0.059, 0.059, 0.141));
		AddLine(points, index, frontColor, backColor, Vec2(-0.101072, -0.835520), Vec2(-0.094078, -0.816636), Vec3(0.592, 0.549, 0.522), Vec3(0.059, 0.059, 0.141), Vec3(0.427, 0.369, 0.341), Vec3(0.067, 0.067, 0.145));
		AddLine(points, index, frontColor, backColor, Vec2(-0.120105, -0.842187), Vec2(-0.100815, -0.835333), Vec3(0.427, 0.369, 0.341), Vec3(0.067, 0.067, 0.145), Vec3(0.541, 0.467, 0.439), Vec3(0.129, 0.129, 0.192));
		AddLine(points, index, frontColor, backColor, Vec2(-0.139116, -0.832593), Vec2(-0.120099, -0.842135), Vec3(0.541, 0.467, 0.439), Vec3(0.129, 0.129, 0.192), Vec3(0.651, 0.561, 0.541), Vec3(0.220, 0.212, 0.298));
		AddLine(points, index, frontColor, backColor, Vec2(-0.182775, -0.748898), Vec2(-0.139277, -0.832422), Vec3(0.651, 0.561, 0.541), Vec3(0.220, 0.212, 0.298), Vec3(0.176, 0.090, 0.090), Vec3(0.329, 0.290, 0.529));
		AddLine(points, index, frontColor, backColor, Vec2(-0.242482, -0.691315), Vec2(-0.182860, -0.748982), Vec3(0.176, 0.090, 0.090), Vec3(0.329, 0.290, 0.529), Vec3(0.376, 0.247, 0.227), Vec3(0.247, 0.212, 0.412));
		AddLine(points, index, frontColor, backColor, Vec2(-0.376186, -0.507215), Vec2(-0.414107, -0.390583), Vec3(0.341, 0.347, 0.365), Vec3(0.071, 0.061, 0.108));
		AddLine(points, index, frontColor, backColor, Vec2(-0.334872, -0.560660), Vec2(-0.375961, -0.507315), Vec3(0.347, 0.353, 0.337), Vec3(0.061, 0.051, 0.096));
		AddLine(points, index, frontColor, backColor, Vec2(-0.234322, -0.652779), Vec2(-0.334981, -0.561046), Vec3(0.333, 0.337, 0.322), Vec3(0.059, 0.047, 0.090), Vec3(0.251, 0.251, 0.255), Vec3(0.067, 0.063, 0.098));
		AddLine(points, index, frontColor, backColor, Vec2(-0.443668, -0.478495), Vec2(-0.425835, -0.437392), Vec3(0.365, 0.322, 0.592), Vec3(0.392, 0.392, 0.510), Vec3(0.353, 0.310, 0.580), Vec3(0.235, 0.227, 0.365));
		AddLine(points, index, frontColor, backColor, Vec2(-0.439128, -0.529675), Vec2(-0.443433, -0.478735), Vec3(0.353, 0.310, 0.580), Vec3(0.235, 0.227, 0.365), Vec3(0.282, 0.275, 0.471), Vec3(0.110, 0.106, 0.200));
		AddLine(points, index, frontColor, backColor, Vec2(-0.375449, -0.627070), Vec2(-0.439486, -0.529481), Vec3(0.282, 0.275, 0.471), Vec3(0.110, 0.106, 0.200), Vec3(0.361, 0.314, 0.592), Vec3(0.110, 0.102, 0.133));
		AddLine(points, index, frontColor, backColor, Vec2(-0.332113, -0.645086), Vec2(-0.375323, -0.626910), Vec3(0.361, 0.314, 0.592), Vec3(0.131, 0.122, 0.145));
		AddLine(points, index, frontColor, backColor, Vec2(-0.265559, -0.648265), Vec2(-0.332002, -0.645122), Vec3(0.361, 0.314, 0.592), Vec3(0.165, 0.153, 0.165));
		AddLine(points, index, frontColor, backColor, Vec2(0.205231, -0.817938), Vec2(0.109033, -0.765411), Vec3(0.429, 0.459, 0.576), Vec3(0.820, 0.737, 0.624));
		AddLine(points, index, frontColor, backColor, Vec2(0.293818, -0.895384), Vec2(0.205808, -0.818524), Vec3(0.447, 0.478, 0.592), Vec3(0.820, 0.737, 0.624), Vec3(0.529, 0.557, 0.659), Vec3(0.820, 0.737, 0.624));
		AddLine(points, index, frontColor, backColor, Vec2(0.354093, -0.996256), Vec2(0.293952, -0.895611), Vec3(0.529, 0.557, 0.659), Vec3(0.820, 0.737, 0.624), Vec3(0.612, 0.635, 0.722), Vec3(0.820, 0.737, 0.624));
		AddLine(points, index, frontColor, backColor, Vec2(0.367427, -1.054936), Vec2(0.353965, -0.995872), Vec3(0.612, 0.635, 0.722), Vec3(0.820, 0.737, 0.624), Vec3(0.725, 0.690, 0.663), Vec3(0.816, 0.737, 0.624));
		AddLine(points, index, frontColor, backColor, Vec2(-0.954948, -0.473706), Vec2(-0.984357, -0.410454), Vec3(0.161, 0.147, 0.363), Vec3(0.398, 0.363, 0.714));
		AddLine(points, index, frontColor, backColor, Vec2(-0.903811, -0.638807), Vec2(-0.954931, -0.473614), Vec3(0.153, 0.141, 0.349), Vec3(0.404, 0.369, 0.718), Vec3(0.067, 0.063, 0.188), Vec3(0.420, 0.392, 0.722));
		AddLine(points, index, frontColor, backColor, Vec2(-0.875055, -0.699107), Vec2(-0.903928, -0.638463), Vec3(0.073, 0.069, 0.202), Vec3(0.408, 0.380, 0.702));
		AddLine(points, index, frontColor, backColor, Vec2(-0.629447, -0.956558), Vec2(-0.687403, -0.870736), Vec3(0.029, 0.029, 0.135), Vec3(0.120, 0.114, 0.286));
		AddLine(points, index, frontColor, backColor, Vec2(-0.605535, -1.023209), Vec2(-0.629620, -0.956694), Vec3(0.029, 0.029, 0.125), Vec3(0.096, 0.096, 0.247));
		AddLine(points, index, frontColor, backColor, Vec2(-0.265320, -0.663013), Vec2(-0.242156, -0.691406), Vec3(0.216, 0.129, 0.125), Vec3(0.361, 0.310, 0.592), Vec3(0.325, 0.267, 0.251), Vec3(0.361, 0.310, 0.592));
		AddLine(points, index, frontColor, backColor, Vec2(-0.265459, -0.648465), Vec2(-0.265186, -0.663040), Vec3(0.355, 0.298, 0.288), Vec3(0.355, 0.306, 0.580));
		AddLine(points, index, frontColor, backColor, Vec2(-0.630864, -0.893412), Vec2(-0.687487, -0.870996), Vec3(0.125, 0.112, 0.275), Vec3(0.259, 0.253, 0.482));
		AddLine(points, index, frontColor, backColor, Vec2(-0.586520, -0.885655), Vec2(-0.630843, -0.893310), Vec3(0.108, 0.092, 0.233), Vec3(0.208, 0.206, 0.418));
		AddLine(points, index, frontColor, backColor, Vec2(-0.487897, -0.819075), Vec2(-0.586679, -0.885778), Vec3(0.104, 0.088, 0.218), Vec3(0.176, 0.176, 0.375));
		AddLine(points, index, frontColor, backColor, Vec2(-0.300556, -0.770950), Vec2(-0.487735, -0.819175), Vec3(0.098, 0.082, 0.188), Vec3(0.188, 0.182, 0.373));
		AddLine(points, index, frontColor, backColor, Vec2(-0.274553, -0.751048), Vec2(-0.300678, -0.771091), Vec3(0.094, 0.078, 0.163), Vec3(0.202, 0.188, 0.363));
		AddLine(points, index, frontColor, backColor, Vec2(-0.242248, -0.691634), Vec2(-0.274548, -0.751130), Vec3(0.090, 0.075, 0.147), Vec3(0.216, 0.196, 0.359));
		AddLine(points, index, frontColor, backColor, Vec2(-0.800847, -0.800581), Vec2(-0.875072, -0.699008), Vec3(0.069, 0.057, 0.225), Vec3(0.351, 0.331, 0.616));
		AddLine(points, index, frontColor, backColor, Vec2(-0.687515, -0.871181), Vec2(-0.800804, -0.800859), Vec3(0.055, 0.049, 0.192), Vec3(0.331, 0.316, 0.584));
		AddLine(points, index, frontColor, backColor, Vec2(-0.727078, 0.837400), Vec2(-0.792967, 0.812525), Vec3(0.851, 0.773, 0.682), Vec3(0.453, 0.390, 0.737));
		AddLine(points, index, frontColor, backColor, Vec2(-0.657094, 0.890944), Vec2(-0.727080, 0.837365), Vec3(0.859, 0.780, 0.682), Vec3(0.475, 0.412, 0.759));
		AddLine(points, index, frontColor, backColor, Vec2(-0.603768, 0.961718), Vec2(-0.657083, 0.890933), Vec3(0.857, 0.780, 0.682), Vec3(0.508, 0.445, 0.794));
		AddLine(points, index, frontColor, backColor, Vec2(-0.589831, 0.999997), Vec2(-0.603747, 0.961733), Vec3(0.857, 0.780, 0.682), Vec3(0.527, 0.467, 0.816));
		AddLine(points, index, frontColor, backColor, Vec2(-0.902586, 0.451620), Vec2(-0.836020, 0.550739), Vec3(0.569, 0.553, 0.867), Vec3(0.618, 0.624, 0.949));
		AddLine(points, index, frontColor, backColor, Vec2(-0.958157, 0.411244), Vec2(-0.902331, 0.451519), Vec3(0.506, 0.494, 0.859), Vec3(0.637, 0.647, 0.957));
		AddLine(points, index, frontColor, backColor, Vec2(-0.988291, 0.406328), Vec2(-0.958317, 0.411469), Vec3(0.465, 0.453, 0.851), Vec3(0.649, 0.663, 0.961));
		AddLine(points, index, frontColor, backColor, Vec2(-0.790561, 0.737649), Vec2(-0.792976, 0.808576), Vec3(0.722, 0.694, 0.898), Vec3(0.859, 0.788, 0.661));
		AddLine(points, index, frontColor, backColor, Vec2(-0.835941, 0.550741), Vec2(-0.790568, 0.737610), Vec3(0.708, 0.680, 0.892), Vec3(0.849, 0.778, 0.680));
		AddLine(points, index, frontColor, backColor, Vec2(0.124987, -0.012010), Vec2(0.128540, -0.077336), Vec3(0.153, 0.094, 0.125), Vec3(0.980, 0.929, 0.725), Vec3(0.082, 0.039, 0.075), Vec3(0.773, 0.251, 0.153));
	}

	/*AddLine(points, index, frontColor, backColor, Vec2(0.5, 0.5), Vec2(-0.5, -0.5), 
		Vec3(1.0, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Vec3(1.0, 0.0, 0.0), Vec3(0.0, 1.0, 0.0));*/
	
	ptrBoundary boundary = make_shared<SegmentBoundaryNew>(points, frontColor, backColor, index, false);
	
	ptrSourceTerm source = make_shared<EmptySource>();
	return PoissonEquation(boundary, source);
}