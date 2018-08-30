#version 330

struct Dual { vec4 u; vec4 v; };

// Conjugate of a quaternion
vec4 conj(vec4 q) { return vec4(q.x, -q.yzw); }
// Dual quaternion conjugate (composed dual and quaternion conjugates)
Dual conj(Dual d) { return Dual(conj(d.u), vec4(-d.v.x, d.v.yzw)); }

// Squared Euclidean norm (omits the square root in q.length())
// Accelerates inverses, combined magnitudes (see next), etc.
float len2(vec4 q) { return q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z; }
// Squared Euclidean norm (see previous)
float len2(Dual d) { return len2(d.u) + len2(d.v); }

// Quaternion product
vec4 mul(vec4 l, vec4 r) {
	return vec4(
		l.x*r.x - l.y*r.y - l.z*r.z - l.w*r.w,
		l.x*r.y + l.y*r.x + l.z*r.w - l.w*r.z,
		l.x*r.z - l.y*r.w + l.z*r.x + l.w*r.y,
		l.x*r.w + l.y*r.z - l.z*r.y + l.w*r.x
	);
}
// Dual product
Dual mul(Dual l, Dual r) { return Dual(mul(l.u, r.u), mul(l.v, r.u) + mul(l.u, r.v)); }
// Quaternion product with dual
Dual mul(vec4 l, Dual r) { return Dual(mul(l, r.u), mul(l, r.v)); }
// Dual product with quaternion
Dual mul(Dual l, vec4 r) { return Dual(mul(l.u, r), mul(l.v, r)); }

// Quaternion division (pre-normalized denominator)
vec4 divn(vec4 n, vec4 d) { return mul(n, conj(d)); }
// Quaternion division (unrestricted denominator)
vec4 div(vec4 n, vec4 d) { return divn(n, d) / len2(d); }
// Dual division by quaternion (pre-normalized denominator)
Dual divn(Dual l, vec4 r) { return Dual(divn(l.u, r), divn(l.v, r)); }
// Dual division by quaternion (unrestricted denominator)
Dual div(Dual l, vec4 r) { return Dual(div(l.u, r), div(l.v, r)); }

// Quaternion 'sandwich' product
vec4 sandwich(vec4 u, vec4 v) { return mul(mul(u, v), conj(u)); }
// Dual 'sandwich' product
Dual sandwich(Dual u, Dual v) { return mul(mul(u, v), conj(u)); }
// Quaternion 'sandwich' product of a dual
Dual sandwich(vec4 l, Dual r) { return mul(mul(l, r), conj(l)); }
// Dual 'sandwich' product of a quaternion
Dual sandwich(Dual l, vec4 r) { return mul(mul(l, r), conj(l)); }

