# Software License Agreement (BSD License)
#
# Copyright (c) 2009, Willow Garage, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#  * Neither the name of Willow Garage, Inc. nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

PKG = 'rxbag_plugins'
import roslib; roslib.load_manifest(PKG)
import rospy

import array
from cStringIO import StringIO
import sys

import Image
import wx
import cairo

def imgmsg_to_pil(img_msg, rgba=True):
    try:
        if img_msg._type == 'sensor_msgs/CompressedImage':
            pil_img = Image.open(StringIO(img_msg.data))
            if pil_img.mode != 'L':
                pil_img = pil_bgr2rgb(pil_img)
        else:
            alpha = False
            if img_msg.encoding == 'mono8':
                mode = 'L'
            elif img_msg.encoding == 'rgb8':
                mode = 'BGR'
            elif img_msg.encoding == 'bgr8':
                mode = 'RGB'
            elif img_msg.encoding in ['bayer_rggb8', 'bayer_bggr8', 'bayer_gbrg8', 'bayer_grbg8']:
                mode = 'L'
            elif img_msg.encoding == 'mono16':
                if img_msg.is_bigendian:
                    mode = 'F;16B'
                else:
                    mode = 'F:16'
            elif img_msg.encoding == 'rgba8':
                mode = 'BGR'
                alpha = True
            elif img_msg.encoding == 'bgra8':
                mode = 'RGB'
                alpha = True
    
            pil_img = Image.frombuffer('RGB', (img_msg.width, img_msg.height), img_msg.data, 'raw', mode, 0, 1)

        if rgba and pil_img.mode != 'RGBA':
            pil_img = pil_img.convert('RGBA')
    
        return pil_img

    except Exception, ex:
        print >> sys.stderr, 'Can\'t convert image: %s' % ex
        return None

def pil_bgr2rgb(pil_img):
    rgb2bgr = (0, 0, 1, 0,
               0, 1, 0, 0,
               1, 0, 0, 0)
    return pil_img.convert('RGB', rgb2bgr)

def imgmsg_to_wx(img_msg):
    # Can use rgb8 encoding directly
    if img_msg.encoding == 'rgb8':
        return wx.ImageFromBuffer(img_msg.width, img_msg.height, img_msg.data)

    pil_img = imgmsg_to_pil(img_msg)
    if not pil_img:
        return None
    
    return wx.ImageFromData(pil_img.size[0], pil_img.size[1], pil_img.tostring())

def pil_to_cairo(pil_img):
    w, h = pil_img.size
    data = array.array('c')
    data.fromstring(pil_img.tostring())

    return cairo.ImageSurface.create_for_data(data, cairo.FORMAT_ARGB32, w, h)

def wxbitmap_to_cairo(bitmap):
    image = wx.ImageFromBitmap(bitmap)
    pil_img = Image.new('RGB', (image.GetWidth(), image.GetHeight()))
    pil_img.fromstring(image.GetData())

    pil_img = pil_bgr2rgb(pil_img)
    if pil_img.mode != 'RGBA':
        pil_img = pil_img.convert('RGBA')
    
    return pil_to_cairo(pil_img)
