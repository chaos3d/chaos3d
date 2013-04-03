template <class Real>
Rectangle<Real>::Rectangle ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Rectangle<Real>::Rectangle (const Vector2<Real>& rkOrigin,
    Real fExtent0, Real fExtent1)
    :
    Origin(rkOrigin)
{
    Extent[0] = fExtent0;
    Extent[1] = fExtent1;
}
//----------------------------------------------------------------------------
template <class Real>
Rectangle<Real>::Rectangle ( Real* values )
    :Origin(values[0],values[1])
{
    Extent[0] = values[2] ;
    Extent[1] = values[3];
}
//----------------------------------------------------------------------------
template <class Real>
void Rectangle<Real>::ComputeVertices (Vector2<Real> akVertex[4]) const
{
    akVertex[0] = Origin;
    akVertex[1] = Origin + Vector2<Real>(Extent[0], (Real)0.0);
    akVertex[2] = Origin + Vector2<Real>(Extent[0], Extent[1]);
    akVertex[3] = Origin + Vector2<Real>((Real)0.0, Extent[1]);
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> Rectangle<Real>::GetPPCorner () const
{
    return Origin + Vector2<Real>(Extent[0], Extent[1]);
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> Rectangle<Real>::GetPMCorner () const
{
    return Origin + Vector2<Real>( Extent[0], (Real)0.0);
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> Rectangle<Real>::GetMPCorner () const
{
    return Origin + Vector2<Real>( (Real)0.0, Extent[1]);
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> Rectangle<Real>::GetMMCorner () const
{
    return Origin;
}
//----------------------------------------------------------------------------
