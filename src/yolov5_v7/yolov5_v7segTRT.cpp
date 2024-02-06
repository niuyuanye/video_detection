#include "cuda_utils.h"
#include "config.h"
#include "logging.h"
#include "utils.h"
#include "calibrator.h"
#include "preprocess.h"
#include "model.h"
#include "postprocess.h"

#include "yolov5_v7segTRT.h"
using namespace nvinfer1;
#define DEVICE 0  // GPU id


// stuff we know about the network and the input/output blobs                                                               
const static int kOutputSize1 = kMaxNumOutputBbox * sizeof(Detection) / sizeof(float) + 1;
const static int kOutputSize2 = 32 * (kInputH / 4) * (kInputW / 4);
static Logger gLogger;
std::unordered_map<int, std::string> labels_map;

IRuntime* runtime = nullptr;
ICudaEngine* engine = nullptr;
IExecutionContext* context = nullptr;
cudaStream_t stream;

void prepare_buffers(ICudaEngine* engine, float** gpu_input_buffer, float** gpu_output_buffer1, float** gpu_output_buffer2, float** cpu_output_buffer1, float** cpu_output_buffer2) {
  assert(engine->getNbBindings() == 3);
  // In order to bind the buffers, we need to know the names of the input and output tensors.
  // Note that indices are guaranteed to be less than IEngine::getNbBindings()
  const int inputIndex = engine->getBindingIndex(kInputTensorName);
  const int outputIndex1 = engine->getBindingIndex(kOutputTensorName);
  const int outputIndex2 = engine->getBindingIndex("proto");
  assert(inputIndex == 0);
  assert(outputIndex1 == 1);
  assert(outputIndex2 == 2);

  // Create GPU buffers on device
  CUDA_CHECK(cudaMalloc((void**)gpu_input_buffer, kBatchSize * 3 * kInputH * kInputW * sizeof(float)));
  CUDA_CHECK(cudaMalloc((void**)gpu_output_buffer1, kBatchSize * kOutputSize1 * sizeof(float)));
  CUDA_CHECK(cudaMalloc((void**)gpu_output_buffer2, kBatchSize * kOutputSize2 * sizeof(float)));

  // Alloc CPU buffers
  *cpu_output_buffer1 = new float[kBatchSize * kOutputSize1];
  *cpu_output_buffer2 = new float[kBatchSize * kOutputSize2];
}

void infer(IExecutionContext& context, cudaStream_t& stream, void **buffers, float* output1, float* output2, int batchSize) {
  context.enqueue(batchSize, buffers, stream, nullptr);
  CUDA_CHECK(cudaMemcpyAsync(output1, buffers[1], batchSize * kOutputSize1 * sizeof(float), cudaMemcpyDeviceToHost, stream));
  CUDA_CHECK(cudaMemcpyAsync(output2, buffers[2], batchSize * kOutputSize2 * sizeof(float), cudaMemcpyDeviceToHost, stream));
  cudaStreamSynchronize(stream);
}

void deserialize_engine(std::string& engine_name, IRuntime** runtime, ICudaEngine** engine, IExecutionContext** context) {
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

int Yolov5_v7segTRT::InitialSeg(std::string engine_name,double NMS_THRESH, double CONF_THRESH){
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
	
    
	
    // deserialize the .engine and run inference
	deserialize_engine(engine_name, &runtime, &engine, &context);
	CUDA_CHECK(cudaStreamCreate(&stream));
  cuda_preprocess_init(kMaxInputImageSize);
  prepare_buffers(engine, &gpu_buffers[0], &gpu_buffers[1], &gpu_buffers[2], &cpu_output_buffer1, &cpu_output_buffer2);

  return 1;

}


cv::Mat  Yolov5_v7segTRT::DetectImg(cv::Mat img, std::vector<std::vector<Detection>> result){
  std::cout << "inference time:   00" << std::endl;
  std::vector<cv::Mat> img_batch;
  img_batch.push_back(img);
  
  // Preprocess
  cuda_batch_preprocess(img_batch, gpu_buffers[0], kInputW, kInputH, stream);
  std::cout << "inference time:   01" << std::endl;
  // Run inference
  auto start = std::chrono::system_clock::now();
  infer(*context, stream, (void**)gpu_buffers, cpu_output_buffer1, cpu_output_buffer2, kBatchSize);
  auto end = std::chrono::system_clock::now();
  std::cout << "inference time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
  // NMS
  std::vector<std::vector<Detection>> res_batch(1);
  batch_nms(res_batch, cpu_output_buffer1, img_batch.size(), kOutputSize1, kConfThresh, kNmsThresh);

  std::cout << "inference time:   02" << std::endl;


  
  auto masks = process_mask(&cpu_output_buffer2[0], 32 * (kInputH / 4) * (kInputW / 4), res_batch[0]);
  std::cout << "inference time:   03" << std::endl;
  draw_mask_bbox(img, res_batch[0], masks, labels_map);
  std::cout << "inference time:   04" << std::endl;
	return img;

}

// cv::Mat Yolov5_v7segTRT::SegImg(cv::Mat img){

// 	std::vector<cv::Mat> img_batch;
// 	img_batch.push_back(img);
  
// 	// Preprocess
//     cuda_batch_preprocess(img_batch, gpu_buffers[0], kInputW, kInputH, stream);
// 	// Run inference
//     auto start = std::chrono::system_clock::now();
//     infer(*context, stream, (void**)gpu_buffers, cpu_output_buffer1, cpu_output_buffer2, kBatchSize);
//     auto end = std::chrono::system_clock::now();
//     std::cout << "inference time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
// 	// NMS
//   std::vector<std::vector<Detection>> res_batch(1);
//   batch_nms(res_batch, cpu_output_buffer1, img_batch.size(), kOutputSize1, kConfThresh, kNmsThresh);
  
// 	auto masks = process_mask(&cpu_output_buffer2[0], 32 * (kInputH / 4) * (kInputW / 4), res_batch[0]);
// 	draw_mask_bbox(img, res_batch[0], masks, labels_map);
// 	return img;
// }

void Yolov5_v7segTRT::Release(){
     // Release stream and buffers
    cudaStreamDestroy(stream);
	  CUDA_CHECK(cudaFree(gpu_buffers[0]));
    CUDA_CHECK(cudaFree(gpu_buffers[1]));
    CUDA_CHECK(cudaFree(gpu_buffers[2]));
    delete[] cpu_output_buffer1;
    delete[] cpu_output_buffer2;
	  cuda_preprocess_destroy();
    // Destroy the engine
    context->destroy();
    engine->destroy();
    runtime->destroy();    
}
