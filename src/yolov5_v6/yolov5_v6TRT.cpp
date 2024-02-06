

// #include "cuda_utils.h"
// #include "logging.h"
// #include "common.hpp"
// #include "utils.h"
// #include "calibrator.h"


#include "cuda_utils.h"
#include "logging.h"
#include "common.hpp"
#include "utils.h"
#include "calibrator.h"
#include "preprocess.h"

#include "yolov5_v6TRT.h"

// #define USE_FP32  // set USE_INT8 or USE_FP16 or USE_FP32
#define DEVICE 0  // GPU id
#define BATCH_SIZE 1
#define MAX_IMAGE_INPUT_SIZE_THRESH 3000 * 3000 // ensure it exceed the maximum size in the input images !

// stuff we know about the network and the input/output blobs                                                               
static const int INPUT_H = Yolo::INPUT_H;
static const int INPUT_W = Yolo::INPUT_W;
static const int CLASS_NUM = Yolo::CLASS_NUM;
static const int OUTPUT_SIZE = Yolo::MAX_OUTPUT_BBOX_COUNT * sizeof(Yolo::Detection) / sizeof(float) + 1;  // we assume the yololayer outputs no more than MAX_OUTPUT_BBOX_COUNT boxes that conf >= 0.1
static Logger gLogger;

static float data[BATCH_SIZE * 3 * INPUT_H * INPUT_W];
static float prob[BATCH_SIZE * OUTPUT_SIZE];


IRuntime* runtime = nullptr;
ICudaEngine* engine = nullptr;
IExecutionContext* context = nullptr;
cudaStream_t stream;

// void doInference(IExecutionContext& context, cudaStream_t& stream, void **buffers, float* input, float* output, int batchSize);
void doInference(IExecutionContext& context, cudaStream_t& stream, void **buffers, float* output, int batchSize);

int Yolov5_v6TRT::InitialDetcet(std::string engine_name,double NMS_THRESH, double CONF_THRESH){
    cudaSetDevice(DEVICE);

    this->NMS_THRESH =NMS_THRESH;
    this->CONF_THRESH =CONF_THRESH;
    // deserialize the .engine and run inference
    // engine_name = "./yolov5s.engine";
    std::ifstream file(engine_name, std::ios::binary);
    if (!file.good()) {
        std::cerr << "read " << engine_name << " error!" << std::endl;
        // LOG_INFO("read "<<engine_name<<" error!");
        return -1;
    }
    char *trtModelStream = nullptr;
    size_t size = 0;
    file.seekg(0, file.end);
    size = file.tellg();
    file.seekg(0, file.beg);
    trtModelStream = new char[size];
    assert(trtModelStream);
    file.read(trtModelStream, size);
    file.close();



    // prepare input data ---------------------------
    // static float data[BATCH_SIZE * 3 * INPUT_H * INPUT_W];
    //for (int i = 0; i < 3 * INPUT_H * INPUT_W; i++)
    //    data[i] = 1.0;
    // static float prob[BATCH_SIZE * OUTPUT_SIZE];

    runtime = createInferRuntime(gLogger);
    assert(runtime != nullptr);
    engine = runtime->deserializeCudaEngine(trtModelStream, size);
    assert(engine != nullptr);
    context = engine->createExecutionContext();
    assert(context != nullptr);
    delete[] trtModelStream;
    assert(engine->getNbBindings() == 2);
    // void* buffers[2];
    // In order to bind the buffers, we need to know the names of the input and output tensors.
    // Note that indices are guaranteed to be less than IEngine::getNbBindings()
    const int inputIndex = engine->getBindingIndex(INPUT_BLOB_NAME);
    const int outputIndex = engine->getBindingIndex(OUTPUT_BLOB_NAME);
    this->inputIndex=inputIndex;
    this->outputIndex= outputIndex;
    assert(inputIndex == 0);
    assert(outputIndex == 1);
    // Create GPU buffers on device
    CUDA_CHECK(cudaMalloc(&buffers[inputIndex], BATCH_SIZE * 3 * INPUT_H * INPUT_W * sizeof(float)));
    CUDA_CHECK(cudaMalloc(&buffers[outputIndex], BATCH_SIZE * OUTPUT_SIZE * sizeof(float)));   

    // Create stream
    // cudaStream_t stream;
    CUDA_CHECK(cudaStreamCreate(&stream));

    // prepare input data cache in pinned memory 
    CUDA_CHECK(cudaMallocHost((void**)&img_host, MAX_IMAGE_INPUT_SIZE_THRESH * 3));
    // prepare input data cache in device memory
    CUDA_CHECK(cudaMalloc((void**)&img_device, MAX_IMAGE_INPUT_SIZE_THRESH * 3));
    // LOG_INFO("read "<<engine_name<<" success!");

}
std::vector<std::vector<Yolo::Detection>> Yolov5_v6TRT::DetectImg(cv::Mat img, std::vector<std::vector<Yolo::Detection>> result){
        // if (img.empty()) continue;
        // int fcount = 1;
        int b = 0;    
        float* buffer_idx = (float*)buffers[this->inputIndex];
        size_t  size_image = img.cols * img.rows * 3;
        size_t  size_image_dst = INPUT_H * INPUT_W * 3;
        //copy data to pinned memory
        memcpy(img_host,img.data,size_image);
        //copy data to device memory
        CUDA_CHECK(cudaMemcpyAsync(img_device,img_host,size_image,cudaMemcpyHostToDevice,stream));
        preprocess_kernel_img(img_device, img.cols, img.rows, buffer_idx, INPUT_W, INPUT_H, stream);



        // doInference(*context, stream, buffers, data, prob, BATCH_SIZE);
        doInference(*context, stream, (void**)buffers, prob, BATCH_SIZE);

        // auto end = std::chrono::system_clock::now();
        // std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;


        std::vector<std::vector<Yolo::Detection>> batch_res(1);
        auto& res = batch_res[0];
        nms( batch_res[0], &prob[b * OUTPUT_SIZE], CONF_THRESH, NMS_THRESH);
        result = batch_res;


        // for (size_t j = 0; j < res.size(); j++) {   
        //     cv::Rect r = get_rect(img, res[j].bbox);
        //     cv::rectangle(img, r, cv::Scalar(0x27, 0xC1, 0x36), 2);
        //     // std::cout << "w:" << r.width << std::endl;
        //     // std::cout << "f:" << r.height << std::endl;
        //     cv::putText(img, std::to_string((int)res[j].class_id), cv::Point(r.x, r.y - 1), cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(0xFF, 0xFF, 0xFF), 2);
        // }
        

        // imshow("test", img);
        // cv::waitKey(1);

        return batch_res;

        // result[0].bbox = res.bbox;
        // // result[0].conf = res.conf;
        // result[0].class_id = res.class_id;
        // return res;

}
void Yolov5_v6TRT::Close(){
     // Release stream and buffers
    cudaStreamDestroy(stream);
    CUDA_CHECK(cudaFree(img_device));
    CUDA_CHECK(cudaFreeHost(img_host));
    CUDA_CHECK(cudaFree(buffers[inputIndex]));
    CUDA_CHECK(cudaFree(buffers[outputIndex]));
    // Destroy the engine
    context->destroy();
    engine->destroy();
    runtime->destroy();    
}

void doInference(IExecutionContext& context, cudaStream_t& stream, void **buffers, float* output, int batchSize) {
    // infer on the batch asynchronously, and DMA output back to host
    context.enqueue(batchSize, buffers, stream, nullptr);
    CUDA_CHECK(cudaMemcpyAsync(output, buffers[1], batchSize * OUTPUT_SIZE * sizeof(float), cudaMemcpyDeviceToHost, stream));
    cudaStreamSynchronize(stream);
}