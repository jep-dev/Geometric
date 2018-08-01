#ifndef INITIALIZER_HPP
#define INITIALIZER_HPP

struct Initializer {
protected:
	void (*const init) (void);
	bool (*const validate) (void);
	void (*const quit) (void);
	bool status = false;
public:
	operator bool(void) const { return status; }
	Initializer(void (*const init)(void), bool (*const validate)(void), void (*const quit)(void)):
			init(init), validate(validate), quit(quit) {
		init();
		status = validate();
	}
	virtual ~Initializer(void) { quit(); }
};

#endif
