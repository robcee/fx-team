/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef NS_SVGPOLYELEMENT_H_
#define NS_SVGPOLYELEMENT_H_

#include "nsIDOMSVGAnimatedPoints.h"
#include "nsSVGPathGeometryElement.h"
#include "SVGAnimatedPointList.h"

typedef nsSVGPathGeometryElement nsSVGPolyElementBase;

class gfxContext;

namespace mozilla {
class DOMSVGPointList;
}

class nsSVGPolyElement : public nsSVGPolyElementBase,
                         public nsIDOMSVGAnimatedPoints
{
protected:
  nsSVGPolyElement(already_AddRefed<nsINodeInfo> aNodeInfo);

public:
  //interfaces
  
  NS_DECL_ISUPPORTS_INHERITED
  NS_DECL_NSIDOMSVGANIMATEDPOINTS

  // nsIContent interface
  NS_IMETHOD_(bool) IsAttributeMapped(const nsIAtom* name) const;

  virtual SVGAnimatedPointList* GetAnimatedPointList() {
    return &mPoints;
  }
  virtual nsIAtom* GetPointListAttrName() const {
    return nsGkAtoms::points;
  }

  // nsSVGPathGeometryElement methods:
  virtual bool AttributeDefinesGeometry(const nsIAtom *aName);
  virtual bool IsMarkable() { return true; }
  virtual void GetMarkPoints(nsTArray<nsSVGMark> *aMarks);
  virtual void ConstructPath(gfxContext *aCtx);

  // WebIDL
  already_AddRefed<mozilla::DOMSVGPointList> Points();
  already_AddRefed<mozilla::DOMSVGPointList> AnimatedPoints();

protected:
  SVGAnimatedPointList mPoints;
};

#endif //NS_SVGPOLYELEMENT_H_
