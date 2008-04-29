/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Corporation code.
 *
 * The Initial Developer of the Original Code is Mozilla Corporation.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Vladimir Vukicevic <vladimir@pobox.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "gfxQPainterSurface.h"
#include "gfxImageSurface.h"

#include "cairo-qpainter.h"

gfxQPainterSurface::gfxQPainterSurface(QPainter *painter)
{
    cairo_surface_t *csurf = cairo_qpainter_surface_create (painter);

    mPainter = painter;

    Init (csurf);
}

gfxQPainterSurface::gfxQPainterSurface(const gfxIntSize& size, gfxImageFormat format)
{
    cairo_surface_t *csurf = cairo_qpainter_surface_create_with_qimage ((cairo_format_t) format,
                                                                        size.width,
                                                                        size.height);
    mPainter = cairo_qpainter_surface_get_qpainter (csurf);

    Init (csurf);
}

gfxQPainterSurface::gfxQPainterSurface(const gfxIntSize& size, gfxContentType content)
{
    cairo_surface_t *csurf = cairo_qpainter_surface_create_with_qpixmap ((cairo_content_t) content,
                                                                         size.width,
                                                                         size.height);
    mPainter = cairo_qpainter_surface_get_qpainter (csurf);

    Init (csurf);
}

gfxQPainterSurface::gfxQPainterSurface(cairo_surface_t *csurf)
{
    mPainter = cairo_qpainter_surface_get_qpainter (csurf);

    Init(csurf, PR_TRUE);
}

gfxQPainterSurface::~gfxQPainterSurface()
{
}

QImage *
gfxQPainterSurface::GetQImage()
{
    if (!mSurfaceValid)
        return nsnull;

    return cairo_qpainter_surface_get_qimage(CairoSurface());
}

already_AddRefed<gfxImageSurface>
gfxQPainterSurface::GetImageSurface()
{
    if (!mSurfaceValid)
        return nsnull;

    cairo_surface_t *isurf = cairo_qpainter_surface_get_image(CairoSurface());
    if (!isurf)
        return nsnull;

    if (cairo_surface_get_type(isurf) == CAIRO_SURFACE_TYPE_IMAGE)
        return nsnull;

    nsRefPtr<gfxImageSurface> asurf = new gfxImageSurface(isurf);
    return asurf.forget();
}
