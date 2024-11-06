namespace blue
{
  inline float tiltcamera::min_distance_to_lookat() const
  {
    return radius_;
  }

  inline float tiltcamera::max_distance_to_lookat() const
  {
    return bounding_radius();
  }

  inline float tiltcamera::bounding_radius() const
  {
    return boundingSphere_.radius();
  }

  inline solosnake::point3d tiltcamera::location() const
  {
    return location_;
  }

  inline solosnake::point3d tiltcamera::lookedat() const
  {
    return lookedat_;
  }
}
