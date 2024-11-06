namespace blue
{
  inline float gamecamera::min_distance_to_lookat() const
  {
    return radius_;
  }

  inline float gamecamera::max_distance_to_lookat() const
  {
    return bounding_radius();
  }

  inline float gamecamera::bounding_radius() const
  {
    return boundingSphere_.radius();
  }

  inline solosnake::point3d gamecamera::location() const
  {
    return location_;
  }

  inline solosnake::point3d gamecamera::lookedat() const
  {
    return lookedat_;
  }
}
