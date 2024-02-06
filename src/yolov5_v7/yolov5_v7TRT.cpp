#include "cuda_utils.h"
#include "config.h"
#include "logging.h"
#include "utils.h"
#include "calibrator.h"
#include "preprocess.h"
#include "model.h"
#include "postprocess.h"

#include "yolov5_v7TRT.h"

using namespace nvinfer1;
#define DEVICE 0  // GPU id
static Logger gLogger;
const static int kOutputSize = kMaxNumOutputBbox * sizeof(Detection) / sizeof(float) + 1;

IRuntime* runtime_det = nullptr;
ICudaEngine* engine_det  = nullptr;
IExecutionContext* context_det  = nullptr;
cudaStream_t stream_det;



void prepare_buffers_det(ICudaEngine* engine, float** gpu_input_buffer, float** gpu_output_buffer, float** cpu_output_buffer) {
  assert(engine->getNbBindings() == 2);
  // In order to bind the buffers, we need to know the names of the input and output tensors.
  // Note that indices are guaranteed to be less than IEngine::getNbBindings()
  const int inputIndex = engine->getBindingIndex(kInputTensorName);
  const int outputIndex = engine->getBindingIndex(kOutputTensorName);
  assert(inputIndex == 0);
  assert(outputIndex == 1);
  // Create GPU buffers on device
  CUDA_CHECK(cudaMalloc((void**)gpu_input_buffer, kBatchSize * 3 * kInputH * kInputW * sizeof(float)));
  CUDA_CHECK(cudaMalloc((void**)gpu_output_buffer, kBatchSize * kOutputSize * sizeof(float)));

  *cpu_output_buffer = new float[kBatchSize * kOutputSize];
}

void infer_det(IExecutionContext& context, cudaStream_t& stream, void** gpu_buffers, float* output, int batchsize) {
  context.enqueue(batchsize, gpu_buffers, stream, nullptr);
  CUDA_CHECK(cudaMemcpyAsync(output, gpu_buffers[1], batchsize * kOutputSize * sizeof(float), cudaMemcpyDeviceToHost, stream));
  cudaStreamSynchronize(stream);
}


void deserialize_engine_det(std::string& engine_name, IRuntime** runtime, ICudaEngine** engine, IExecutionContext** context) {
  std::ifstream file(engine_name, std::ios::binary);
  if (!file.good()) {
    std::cerr << "read " << engine_name << " error!" << std::endl;
    assert(false);
  }
  size_t size = 0;
  file.seekg(0, file.end);
  size = file.tellg();
  file.seekg(0, file.beg);
  char* serialized_engine = new char[size];
  assert(serialized_engine);
  file.read(serialized_engine, size);
  file.close();

  *runtime = createInferRuntime(gLogger);
  assert(*runtime);
  *engine = (*runtime)->deserializeCudaEngine(serialized_engine, size);
  assert(*engine);
  *context = (*engine)->createExecutionContext();
  assert(*context);
  delete[] serialized_engine;
}


int Yolov5_v7TRT::InitialDetcet(std::string engine_name,double NMS_THRESH, double CONF_THRESH){
  this->kNmsThresh = NMS_THRESH;
  this->kConfThresh = CONF_THRESH;


	CUDA_CHECK(cudaSetDevice(0));
 
	std::string labels_filename = "./coco.txt";
	std::ifstream labels_file(labels_filename, std::ios::binary);
    if (!labels_file.good())
	{
		std::cerr << "read " << labels_filename << " error!" << std::endl;
		return -1;
    }
	read_labels(labels_filename, labels_map);	
    
  deserialize_engine_det(engine_name, &runtime_det , &engine_det , &context_det );
  // cudaStream_t stream;
  CUDA_CHECK(cudaStreamCreate(&stream_det));

    // Init CUDA preprocessing
  cuda_preprocess_init(kMaxInputImageSize);

  // Prepare cpu and gpu buffers

  prepare_buffers_det(engine_det , &gpu_buffers[0], &gpu_buffers[1], &cpu_output_buffer);

  return 1;

}


cv::Mat  Yolov5_v7TRT::DetectImg(cv::Mat img, std::vector<std::vector<Detection>> result){

  std::vector<cv::Mat> img_batch;
  img_batch.push_back(img);
  
  // Preprocess
cuda_batch_preprocess(img_batch, gpu_buffers[0], kInputW, kInputH, stream_det);

// Run inference
auto start = std::chrono::system_clock::now();
infer_det(*context_det , stream_det, (void**)gpu_buffers, cpu_output_buffer, kBatchSize);
auto end = std::chrono::system_clock::now();
std::cout << "inference time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

// NMS
std::vector<std::vector<Detection>> res_batch;
batch_nms(res_batch, cpu_output_buffer, img_batch.size(), kOutputSize, this->kConfThresh, this->kNmsThresh);

// Draw bounding boxes
draw_bbox(img_batch, res_batch);



return img;

}

void Yolov5_v7TRT::Release(){
     // Release stream and buffers
    cudaStreamDestroy(stream_det);
    CUDA_CHECK(cudaFree(gpu_buffers[0]));
    CUDA_CHECK(cudaFree(gpu_buffers[1]));
    delete[] cpu_output_buffer;
    cuda_preprocess_destroy();
    // Destroy the engine
    context_det ->destroy();
    engine_det ->destroy();
    runtime_det ->destroy();
}
