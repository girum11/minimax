/*
 * PylosView.h
 *
 *  Created on: Oct 29, 2012
 *      Author: girum
 */

#ifndef PYLOSVIEW_H_
#define PYLOSVIEW_H_

#include "View.h"

class PylosView : public View {
public:
	void Draw(std::ostream &out);
	const Class *GetClass() const;

};

#endif /* PYLOSVIEW_H_ */
