# blank-cpp-api
Pistache + Env variables + Json + Json input validator + SQL Snippets

This project is for tests and study
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.



### Docker build multi-stage + AWS Lambda packager
Testing building with Ubuntu and deploying in a Alpine image using the AWS Lambda Packager script to get the dependency libraries.
The packager script was modified to facilitate the use in the build container

* Installing the dependencies and building in the Ubuntu container
* Using the AWS Lambda C++ packager to get the dependency libraries including the Ubuntu's GLIBC
* Running the project in a Alpine container
