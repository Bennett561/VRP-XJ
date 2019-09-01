#pragma once
struct pair_hash {
	template <class T1, class T2>
	std::size_t operator () (const std::pair<T1, T2> &p) const {
		auto h1 = std::hash<T1>{}(get<0>(p));
		auto h2 = std::hash<T2>{}(get<1>(p));

		return h1 ^ h2;
	}
};

struct tuple_hash3 {
	template <class T1, class T2, class T3>
	std::size_t operator () (const std::tuple<T1, T2, T3> &p) const {
		auto h1 = std::hash<T1>{}(get<0>(p));
		auto h2 = std::hash<T2>{}(get<1>(p));
		auto h3 = std::hash<T3>{}(get<2>(p));

		return h1 ^ h2 ^ h3;
	}
};

struct tuple_hash4 {
	template <class T1, class T2, class T3, class T4>
	std::size_t operator () (const std::tuple<T1, T2, T3, T4> &p) const {
		auto h1 = std::hash<T1>{}(get<0>(p));
		auto h2 = std::hash<T2>{}(get<1>(p));
		auto h3 = std::hash<T3>{}(get<2>(p));
		auto h4 = std::hash<T4>{}(get<3>(p));

		return h1 ^ h2 ^ h3 ^ h4;
	}
};