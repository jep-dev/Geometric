#ifndef TIMING_HPP
#define TIMING_HPP

#include <chrono>
#include <thread>

namespace Timing {
	using namespace std::chrono;
	using clock = high_resolution_clock;

	template<class T>
	struct Delta {
		typedef std::pair<double, double> Measure;
		typedef std::pair<unsigned, Measure> Iteration;
		typedef std::vector<Iteration> Iterations;
		typedef T& (Action)(unsigned, T&);
		static double elapsed(Delta<T> const& d, unsigned N, T& x) {
			using namespace std::chrono;
			using clock = high_resolution_clock;
			auto t0 = clock::now();
			d.action(N, x);
			auto t1 = clock::now();
			return duration_cast<duration<double>>(t1-t0).count() * d.uScale;
		}
	protected:
		Iterations iterations;
		Iteration maxes = {0, {0, 0}};
	public:
		/** Decides how to spend N iterations altering x. */
		Action *action;
		/** Decides how to scale measurements. */
		unsigned uScale;
		/** Decides how to label scaled measurements. */
		const char *szScale;
		static T& multiply(unsigned N, T &x)
			{ for(auto l = 0; l < N; l++) x = x * x; return x; }
		static T& add(unsigned N, T &x)
			{ for(auto l = 0; l < N; l++) x = x + x; return x; }
		static T& conjugate(unsigned N, T &x)
			{ for(auto l = 0; l < N; l++) x = *x; return x; }
		Delta(T& t, Action *action = multiply, unsigned uScale = 100000, const char *szScale = "ns"):
			action(action), uScale(uScale), szScale(szScale) {}

		template<class... R>
		Delta(T& x, int M, R &&... r): Delta(x, std::forward<R>(r)...) {
			using namespace std::chrono;

			unsigned N = std::max(0, M);
			maxes.first = std::max(maxes.first, N);

			auto t0 = high_resolution_clock::now();
			action(N, x);
			auto t1 = high_resolution_clock::now();

			double dt = duration_cast<duration<double>>(t1 - t0).count() * uScale, dt_dl = dt / N;
			auto& s = maxes.second;
			s.first = std::max(s.first, dt_dl);
			s.second = std::max(s.second, dt);
			iterations.emplace_back(N, std::make_pair(dt_dl, dt));
		}
		template<class S>
		friend S& operator<<(S &s, Delta const& d) {
			using namespace std;
			auto f = s.flags();
			string labelLeft = "Loop ", labelRight = ", ";
			unsigned precision = 4,
				loop = 1, loops = d.iterations.size(),
				loopDigits = ceil(log10(loops+1)), nDigits, singleDigits, totalDigits,
				labelWidth = labelLeft.length() + loopDigits + labelRight.length(),
				singleWidth, totalWidth;
			{
				nDigits = ceil(log10(d.maxes.first + 1));
				auto const& sec = d.maxes.second;
				unsigned single = sec.first, total = sec.second;
				singleDigits = ceil(log10(single+1));
				totalDigits = ceil(log10(total+1));
			}
			singleWidth = precision + 1 + singleDigits;
			totalWidth = precision + 1 + totalDigits;
			// Iterate in reverse to restore original argument order
			for(auto it = d.iterations.crbegin(); it < d.iterations.crend(); it++, loop++) {
				auto const& sec = it -> second;
				s << labelLeft << setw(loopDigits) << loop << labelRight
					<< setw(nDigits) << it -> first << " ops: ";
				s << fixed << setprecision(precision);
				{
					auto fill = s.fill();
					s << setw(singleWidth) << setfill(fill)
						<< sec.first << ' ' << d.szScale << "/op ("
						<< setw(totalWidth) << setfill(fill)
						<< sec.second << ' ' << d.szScale << " total)" << endl;
				}
				s.setf(f);
			}
			return s;
		}
	};
	template<class P, class... Q>
	Delta<P> delta(P &p, Q &&... q)
		{ return {p, std::forward<Q>(q)...}; }
	template<class P, class... Q>
	Delta<P> delta(P && p, Q &&... q)
		{ return {p, std::forward<Q>(q)...}; }
}

#endif
