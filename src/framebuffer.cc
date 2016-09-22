#include "framebuffer.h"

using namespace v8;

Nan::Persistent<Function> FrameBuffer::constructor;

void FrameBuffer::Init() {
    Nan::HandleScope scope;

    Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(FrameBuffer::New);
    ctor->InstanceTemplate()->SetInternalFieldCount(1);
    ctor->SetClassName(Nan::New("FrameBuffer").ToLocalChecked());

    ctor->PrototypeTemplate()->Set(Nan::New("size").ToLocalChecked(),
      Nan::New<FunctionTemplate>(Size));
    ctor->PrototypeTemplate()->Set(Nan::New("data").ToLocalChecked(),
      Nan::New<FunctionTemplate>(Data));
    ctor->PrototypeTemplate()->Set(Nan::New("clear").ToLocalChecked(),
      Nan::New<FunctionTemplate>(Clear));
    ctor->PrototypeTemplate()->Set(Nan::New("blit").ToLocalChecked(),
      Nan::New<FunctionTemplate>(Blit));
    ctor->PrototypeTemplate()->Set(Nan::New("color").ToLocalChecked(),
      Nan::New<FunctionTemplate>(Color));
    ctor->PrototypeTemplate()->Set(Nan::New("fill").ToLocalChecked(),
      Nan::New<FunctionTemplate>(Fill));
    ctor->PrototypeTemplate()->Set(Nan::New("line").ToLocalChecked(),
      Nan::New<FunctionTemplate>(Line));
    ctor->PrototypeTemplate()->Set(Nan::New("rect").ToLocalChecked(),
      Nan::New<FunctionTemplate>(Rect));
    ctor->PrototypeTemplate()->Set(Nan::New("circle").ToLocalChecked(),
      Nan::New<FunctionTemplate>(Circle));
    ctor->PrototypeTemplate()->Set(Nan::New("font").ToLocalChecked(),
      Nan::New<FunctionTemplate>(Font));
    ctor->PrototypeTemplate()->Set(Nan::New("text").ToLocalChecked(),
      Nan::New<FunctionTemplate>(Text));
    ctor->PrototypeTemplate()->Set(Nan::New("png").ToLocalChecked(),
      Nan::New<FunctionTemplate>(ImagePNG));
    ctor->PrototypeTemplate()->Set(Nan::New("jpeg").ToLocalChecked(),
      Nan::New<FunctionTemplate>(ImageJPEG));

    constructor.Reset(ctor->GetFunction());

}

Local<Object> FrameBuffer::NewInstance(Local<Value> arg, Local<Value> arg2) {

  Nan::EscapableHandleScope scope;

  const unsigned argc = 2;
  Local<Value> argv[argc] = { arg, arg2 };
  Local<Function> cons = Nan::New<Function>(constructor);
  Local<Object> instance = cons->NewInstance(argc, argv);

  return scope.Escape(instance);

}

NAN_METHOD(FrameBuffer::New) {
    Nan::HandleScope scope;

    v8::String::Utf8Value path(info[0]->ToString());
    std::string _path = std::string(*path);

    FrameBuffer *obj = new FrameBuffer(_path.c_str());
    obj->drawToBuffer = info[1]->IsUndefined() ? false : info[1]->BooleanValue();

    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
}

NAN_METHOD(FrameBuffer::Size) {
    Nan::HandleScope scope;

    FrameBuffer *obj = Nan::ObjectWrap::Unwrap<FrameBuffer>(info.Holder());

    Local<Object> sizeObject = Nan::New<Object>();

    sizeObject->Set(Nan::New<String>("width").ToLocalChecked(), Nan::New<Number>(obj->vinfo.xres));
    sizeObject->Set(Nan::New<String>("height").ToLocalChecked(), Nan::New<Number>(obj->vinfo.yres));

    info.GetReturnValue().Set(sizeObject);
}

NAN_METHOD(FrameBuffer::Data) {
    Nan::HandleScope scope;

    FrameBuffer *obj = Nan::ObjectWrap::Unwrap<FrameBuffer>(info.Holder());

    Local<Object> bufferObject = Nan::NewBuffer(obj->fbp, obj->screenSize).ToLocalChecked();

    info.GetReturnValue().Set(bufferObject);
}

NAN_METHOD(FrameBuffer::Clear) {
    Nan::HandleScope scope;

    FrameBuffer *obj = Nan::ObjectWrap::Unwrap<FrameBuffer>(info.Holder());

    cairo_t *cr = getDrawingContext(obj);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    cairo_destroy(cr);

    return;
}

NAN_METHOD(FrameBuffer::Blit) {
    Nan::HandleScope scope;

    FrameBuffer *obj = Nan::ObjectWrap::Unwrap<FrameBuffer>(info.Holder());

    if (obj->drawToBuffer) {
        cairo_t *cr = cairo_create (obj->screenSurface);
        cairo_set_source_surface (cr, obj->bufferSurface, 0, 0);
        cairo_paint (cr);

        cairo_destroy(cr);
    }

    return;
}

NAN_METHOD(FrameBuffer::Color) {
    Nan::HandleScope scope;

    FrameBuffer *obj = Nan::ObjectWrap::Unwrap<FrameBuffer>(info.Holder());

    obj->r = (info[0]->NumberValue());
    obj->g = (info[1]->NumberValue());
    obj->b = (info[2]->NumberValue());

    return;
}

NAN_METHOD(FrameBuffer::Fill) {
    Nan::HandleScope scope;

    FrameBuffer *obj = Nan::ObjectWrap::Unwrap<FrameBuffer>(info.Holder());

    cairo_t *cr = getDrawingContext(obj);

    cairo_set_source_rgb(cr, obj->r, obj->g, obj->b);
    cairo_paint(cr);

    cairo_destroy(cr);

    return;
}

NAN_METHOD(FrameBuffer::Line) {
    Nan::HandleScope scope;

    double x0 = (info[0]->NumberValue());
    double y0 = (info[1]->NumberValue());
    double x1 = (info[2]->NumberValue());
    double y1 = (info[3]->NumberValue());

    double w = info[4]->IsUndefined() ? 1 : info[4]->NumberValue();

    FrameBuffer *obj = Nan::ObjectWrap::Unwrap<FrameBuffer>(info.Holder());

    cairo_t *cr = getDrawingContext(obj);

    cairo_set_source_rgb(cr, obj->r, obj->g, obj->b);

    cairo_move_to(cr, x0, y0);
    cairo_line_to(cr, x1, y1);

    cairo_set_line_width(cr, w);
    cairo_stroke(cr);

    cairo_destroy(cr);

    return;
}

NAN_METHOD(FrameBuffer::Rect) {
    Nan::HandleScope scope;

    double x = (info[0]->NumberValue());
    double y = (info[1]->NumberValue());
    double w = (info[2]->NumberValue());
    double h = (info[3]->NumberValue());

    FrameBuffer *obj = Nan::ObjectWrap::Unwrap<FrameBuffer>(info.Holder());

    cairo_t *cr = getDrawingContext(obj);

    cairo_set_source_rgb(cr, obj->r, obj->g, obj->b);

    cairo_rectangle(cr, x, y, w, h);

    if (!info[4]->IsUndefined() && info[4]->BooleanValue() == false) {
        double w = info[5]->IsUndefined() ? 1 : info[5]->NumberValue();
        cairo_set_line_width(cr, w);
        cairo_stroke(cr);
    } else if (!info[4]->IsUndefined() && info[4]->BooleanValue() == true) {
        cairo_fill(cr);
    } else {
        cairo_fill(cr);
    }

    cairo_destroy(cr);

    return;
}

NAN_METHOD(FrameBuffer::Circle) {
    Nan::HandleScope scope;

    double x = (info[0]->NumberValue());
    double y = (info[1]->NumberValue());
    double radius = (info[2]->NumberValue());

    FrameBuffer *obj = Nan::ObjectWrap::Unwrap<FrameBuffer>(info.Holder());

    cairo_t *cr = getDrawingContext(obj);

    cairo_set_source_rgb(cr, obj->r, obj->g, obj->b);

    cairo_arc(cr, x, y, radius, 0, 2*3.141592654);

    if (!info[3]->IsUndefined() && info[3]->BooleanValue() == false) {
        double w = info[4]->IsUndefined() ? 1 : info[4]->NumberValue();
        cairo_set_line_width(cr, w);
        cairo_stroke(cr);
    } else if (!info[3]->IsUndefined() && info[3]->BooleanValue() == true) {
        cairo_fill(cr);
    } else {
        cairo_fill(cr);
    }

    cairo_destroy(cr);

    return;
}

NAN_METHOD(FrameBuffer::Font) {
    Nan::HandleScope scope;

    FrameBuffer *obj = Nan::ObjectWrap::Unwrap<FrameBuffer>(info.Holder());

    v8::String::Utf8Value fontName(info[0]->ToString());
    std::string _fontName = std::string(*fontName);

    obj->fontName = _fontName.c_str();
    obj->fontSize = info[1]->IsUndefined() ? 12 : info[1]->NumberValue();
    obj->fontBold = info[2]->IsUndefined() ? false : info[2]->BooleanValue();

    return;
}

NAN_METHOD(FrameBuffer::Text) {
    Nan::HandleScope scope;

    double x = (info[0]->NumberValue());
    double y = (info[1]->NumberValue());

    v8::String::Utf8Value text(info[2]->ToString());
    std::string _text = std::string(*text);

    bool textCentered = info[3]->IsUndefined() ? false : info[3]->BooleanValue();
    double textRotation = info[4]->IsUndefined() ? 0 : info[4]->NumberValue();

    FrameBuffer *obj = Nan::ObjectWrap::Unwrap<FrameBuffer>(info.Holder());

    cairo_t *cr = getDrawingContext(obj);

    cairo_set_source_rgb(cr, obj->r, obj->g, obj->b);

    if (obj->fontBold) {
        cairo_select_font_face(cr, obj->fontName, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    } else {
        cairo_select_font_face(cr, obj->fontName, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    }

    cairo_set_font_size(cr, obj->fontSize);

    cairo_translate(cr, x, y);

    if (textRotation != 0) {
        cairo_rotate(cr, textRotation / (180.0 / 3.141592654));
    }

    if (textCentered) {
        cairo_text_extents_t extents;
        cairo_text_extents(cr, _text.c_str(), &extents);

        cairo_move_to(cr, -extents.width/2, extents.height/2);
    }

    cairo_show_text(cr, _text.c_str());

    cairo_destroy(cr);

    return;
}

NAN_METHOD(FrameBuffer::ImagePNG) {
    Nan::HandleScope scope;

    double x = (info[0]->NumberValue());
    double y = (info[1]->NumberValue());

    v8::String::Utf8Value path(info[2]->ToString());
    std::string _path = std::string(*path);

    FrameBuffer *obj = Nan::ObjectWrap::Unwrap<FrameBuffer>(info.Holder());

    cairo_t *cr = getDrawingContext(obj);

    cairo_surface_t *image = cairo_image_surface_create_from_png(_path.c_str());
    cairo_set_source_surface(cr, image, x, y);
    cairo_paint(cr);

    cairo_status_t status = cairo_status(cr);

    if (status != CAIRO_STATUS_SUCCESS) {
        Nan::ThrowError("Error reading image");
    }

    cairo_surface_destroy(image);
    cairo_destroy(cr);

    return;
}

NAN_METHOD(FrameBuffer::ImageJPEG) {
    Nan::HandleScope scope;

    double x = (info[0]->NumberValue());
    double y = (info[1]->NumberValue());

    v8::String::Utf8Value path(info[2]->ToString());
    std::string _path = std::string(*path);

    write_fb_jpeg(_path.c_str(), (int)x, (int)y);

    return;
}

cairo_t* FrameBuffer::getDrawingContext(FrameBuffer *obj) {
    if (obj->drawToBuffer) {
        return cairo_create(obj->bufferSurface);
    } else {
        return cairo_create(obj->screenSurface);
    }
}

FrameBuffer::FrameBuffer(const char *path) {
    fbfd = open(path, O_RDWR);
    if (fbfd == -1) {
        Nan::ThrowError("Error opening framebuffer device");
        return;
    }

    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        Nan::ThrowError("Error retrieving data from framebuffer");
        return;
    }

    memcpy(&orig_vinfo, &vinfo, sizeof(struct fb_var_screeninfo));

    vinfo.bits_per_pixel = 8;
    if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo)) {
        Nan::ThrowError("Error sending data to framebuffer");
        return;
    }

    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        Nan::ThrowError("Error retrieving data from framebuffer");
        return;
    }

    screenSize = finfo.smem_len;
    fbp = (char*)mmap(0,
                      screenSize,
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED,
                      fbfd,
                      0);

    bbp = (char *)malloc(screenSize);

    if ((int)fbp == -1) {
        Nan::ThrowError("Error during memory mapping");
        return;
    }

    bufferSurface = cairo_image_surface_create_for_data ((unsigned char *)bbp, CAIRO_FORMAT_RGB16_565, vinfo.xres, vinfo.yres, cairo_format_stride_for_width(CAIRO_FORMAT_RGB16_565, vinfo.xres));

    if (cairo_surface_status(bufferSurface) != CAIRO_STATUS_SUCCESS) {
        Nan::ThrowError("Error creating buffer surface");
        return;
    }

    screenSurface = cairo_image_surface_create_for_data ((unsigned char *)fbp, CAIRO_FORMAT_RGB16_565, vinfo.xres, vinfo.yres, cairo_format_stride_for_width(CAIRO_FORMAT_RGB16_565, vinfo.xres));

    if (cairo_surface_status(bufferSurface) != CAIRO_STATUS_SUCCESS) {
        Nan::ThrowError("Error creating screeh surface");
    }
}

FrameBuffer::~FrameBuffer() {
    if ((int)fbp != -1) {
        free(bbp);
        munmap(fbp, screenSize);

        if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &orig_vinfo)) {
            Nan::ThrowError("Error restoring framebuffer state");
        }
    }

    if (fbfd != -1) {
        close(fbfd);
    }

    if (cairo_surface_status(bufferSurface) == CAIRO_STATUS_SUCCESS) {
        cairo_surface_destroy(bufferSurface);
    }

    if (cairo_surface_status(screenSurface) == CAIRO_STATUS_SUCCESS) {
        cairo_surface_destroy(screenSurface);
    }
}

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
int fbfd = 0;
char *fbp = 0;
long int screensize = 0;


int open_raw_fb()
{
    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }

    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");

    return 0;
}

int close_raw_fb()
{
    munmap(fbp, screensize);
    close(fbfd);
    fbp = 0;
    return 0;
}

int write_fb_jpeg(const char *jpegFile, uint8_t xPos, uint8_t yPos)
{
    int size;
    char *buf;
    FILE *f;
    long int location = 0;

    if(!fbp) open_raw_fb();

    f = fopen(jpegFile, "rb");
    if (!f) {
        printf("Error opening the input file: %s.\n", jpegFile);
        return 1;
    }
    fseek(f, 0, SEEK_END);
    size = (int) ftell(f);
    buf = (char *)malloc(size);
    fseek(f, 0, SEEK_SET);
    size = (int) fread(buf, 1, size, f);
    fclose(f);

    njInit();
    if (njDecode(buf, size)) {
        printf("Error decoding the input file.\n");
        return 1;
    }

    uint8_t *img = njGetImage();
    uint16_t jw = njGetWidth();
    uint16_t jh = njGetHeight();
    uint8_t ox, oy;

    int w = vinfo.xres;
    int h = vinfo.yres;

    if(w > jw) w = jw;
    if(h > jh) h = jh;

    for(oy = 0; oy < h - yPos; oy++)
    {
        for(ox = 0; ox < w - xPos; ox++)
        {
            uint16_t i = (uint16_t)oy * jw * 3 + (uint16_t)ox * 3;

            location = (ox+vinfo.xoffset+xPos) * (vinfo.bits_per_pixel/8) +
                       (oy+vinfo.yoffset+yPos) * finfo.line_length;

            int b = img[i + 2]>>3;
            int g = img[i + 1]>>2;
            int r = img[i + 0]>>3;
            unsigned short int t = r<<12 | g << 6 | b;
            //16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01
            *((unsigned short int*)(fbp + location)) = t;
        }
    }
    njDone();

    return 0;
}


