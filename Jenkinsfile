pipeline {
     agent any
     stages {
         stage('AWS Credentials') {
             steps {
                withCredentials([[$class: 'AmazonWebServicesCredentialsBinding', credentialsId: 'bg', accessKeyVariable: 'ACCESS_KEY', secretKeyVariable: 'SECRET_KEY']]){
                 sh '''
                     mkdir -p ~/.aws
                     echo "[default]" > ~/.aws/credentials
                     echo "aws_access_key_id = ${ACCESS_KEY}" >> ~/.aws/credentials
                     echo "aws_secret_access_key = ${SECRET_KEY}" >> ~/.aws/credentials
                     echo "[default]" > ~/.boto
                     echo "aws_access_key_id = ${ACCESS_KEY}" >> ~/.boto
                     echo "aws_secret_access_key = ${SECRET_KEY}" >> ~/.boto
                 '''
                }
             }
         }
         stage('Lint HTML') {
              steps {
                  sh '''
                     echo "Validating Dockerfile and app python file..."
                     hadolint Dockerfile
                     
                     '''
              }
         }
         stage('Deploy EKS cluster') {
              steps {
                  sh  'ansible-playbook -i inventory main.yml'
              } 
         }
         stage('Deploy Docker to EKS') {
              steps {
                  sh  './deploy.sh'
              } 
         }
     }
}
