/*
 * Copyright (c) 2010, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "rviz_renderer_ogre/camera.h"
#include "rviz_renderer_ogre/convert.h"

#include <OGRE/OgreCamera.h>

using namespace rviz_math;

namespace rviz_renderer_ogre
{

Camera::Camera(Ogre::Camera* cam)
: cam_(cam)
{
}

void Camera::setPosition(const Vector3& v)
{
  cam_->setPosition(convert(v));
}

void Camera::setOrientation(const Quaternion& q)
{
  cam_->setOrientation(convert(q));
}

void Camera::lookAt(const Vector3& v)
{
  cam_->lookAt(convert(v));
}

void Camera::setFOVY(float fovy)
{
  cam_->setFOVy(Ogre::Radian(fovy));
}

void Camera::setAspectRatio(float aspect)
{
  cam_->setAspectRatio(aspect);
}

void Camera::setAutoAspectRatio(bool autoratio)
{
  cam_->setAutoAspectRatio(autoratio);
}

void Camera::setNearClipDistance(float dist)
{
  cam_->setNearClipDistance(dist);
}

void Camera::setFarClipDistance(float dist)
{
  cam_->setFarClipDistance(dist);
}

void Camera::move(const Vector3& v)
{
  cam_->move(convert(v));
}

void Camera::moveRelative(const Vector3& v)
{
  cam_->moveRelative(convert(v));
}

void Camera::rotate(const Quaternion& q)
{
  cam_->rotate(convert(q));
}

void Camera::rotateRelative(const Quaternion& q)
{
  cam_->setOrientation(cam_->getOrientation() * convert(q));
}

} // namespace rviz_renderer_ogre
