#include "openai_api.h"

const std::string OpenAIApi::LIST_MODELS = "/v1/models";
const std::string OpenAIApi::RETRIEVE_MODEL = "/v1/models/%s";
const std::string OpenAIApi::CREATE_COMPLETION = "/v1/completions";
const std::string OpenAIApi::CREATE_CHAT_COMPLETION = "/v1/chat/completions";
const std::string OpenAIApi::CREATE_EDIT = "/v1/edits";
const std::string OpenAIApi::CREATE_IMAGE = "/v1/images/generations";
const std::string OpenAIApi::CREATE_IMAGE_EDIT = "/v1/images/edits";
const std::string OpenAIApi::CREATE_IMAGE_VARIATION = "/v1/images/variations";
const std::string OpenAIApi::CREATE_EMBEDDINGS = "/v1/embeddings";
const std::string OpenAIApi::CREATE_TRANSCRIPTION = "/v1/audio/transcriptions";
const std::string OpenAIApi::CREATE_TRANSLATION = "/v1/audio/translations";
const std::string OpenAIApi::LIST_UPLOAD_FILES = "/v1/files";
const std::string OpenAIApi::DELETE_RETRIEVE_FILE = "/v1/files/%s";
const std::string OpenAIApi::RETRIEVE_FILE_CONTENT = "/v1/files/%s/content";
const std::string OpenAIApi::CREATE_LIST_FINE_TUNE = "/v1/fine-tunes";
const std::string OpenAIApi::RETRIEVE_FINE_TUNE = "/v1/fine-tunes/%s";
const std::string OpenAIApi::CANCEL_FINE_TUNE = "/v1/fine-tunes/%s/cancel";
const std::string OpenAIApi::LIST_FINE_TUNE_EVENTS = "/v1/fine-tunes/%s/events";
const std::string OpenAIApi::DELETE_FINE_TUNE_MODEL = "/v1/models/%s";
const std::string OpenAIApi::CREATE_MODERATION = "/v1/moderations";