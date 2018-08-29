#version 330

struct Dual { vec4 u, v; };

// Conjugate of a quaternion
vec4 qconj(vec4 q) { return vec4(q.w, -q.xyz); }

// Squared Euclidean norm (omits the square root in q.length())
float qlen2(vec4 q) { return q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z; }

// Quaternion product
vec4 qmul(vec4 l, vec4 r) {
	return vec4(
		l.w*r.w - l.x*r.x - l.y*r.y - l.z*r.z,
		l.w*r.x + l.x*r.w + l.y*r.z - l.z*r.y,
		l.w*r.y - l.x*r.z + l.y*r.w + l.z*r.x,
		l.w*r.z + l.x*r.y - l.y*r.x + l.z*r.w
	);
}

// Quaternion division (pre-normalized denominator)
vec4 qdivn(vec4 n, vec4 d) { return qmul(n, qconj(d)); }
// Quaternion division (unrestricted denominator)
vec4 qdiv(vec4 n, vec4 d) { return qdivn(n, d) / qlen2(d); }
// Quaternion 'sandwich' product
vec4 qsandwich(vec4 u, vec4 v) { return qmul(qmul(u, v), qconj(u)); }

// Dual quaternion conjugate (combined dual conjugate and quaternion conjugate)
Dual dconj(Dual d) { return Dual(qconj(d.u), -qconj(d.v)); }
// Dual multiplication
Dual dmul(Dual l, Dual r) { return Dual(qmul(l.u, r.u), qmul(l.v, r.u) + qmul(l.u, r.v)); }
// Dual division by quaternion (pre-normalized denominator)
Dual ddivn(Dual l, vec4 r) { return Dual(qdivn(l.u, r), qdivn(l.v, r)); }
// Dual division by quaternion (unrestricted denominator)
Dual ddiv(Dual l, vec4 r) { return Dual(qdiv(l.u, r), qdiv(l.v, r)); }
// Dual 'sandwich' product
Dual dsandwich(Dual u, Dual v) { return dmul(dmul(u, v), dconj(u)); }
